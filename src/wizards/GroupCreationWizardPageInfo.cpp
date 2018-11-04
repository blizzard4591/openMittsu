#include "src/wizards/GroupCreationWizardPageInfo.h"
#include "ui_groupcreationwizardpageinfo.h"

#include "src/widgets/ContactListWidgetItem.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/QObjectConnectionMacro.h"

#include <QMessageBox>

namespace openmittsu {
	namespace wizards {
		GroupCreationWizardPageInfo::GroupCreationWizardPageInfo(QHash<openmittsu::protocol::ContactId, openmittsu::database::ContactData> const& knownIdentitiesWithNicknamesExcludingSelfContactId, std::unique_ptr<openmittsu::dataproviders::GroupCreationProcessor> groupCreationProcessor, QWidget* parent) : QWizardPage(parent), m_ui(new Ui::GroupCreationWizardPageInfo), m_knownIdentities(knownIdentitiesWithNicknamesExcludingSelfContactId), m_groupCreationProcessor(std::move(groupCreationProcessor)) {
			m_ui->setupUi(this);

			m_nameValidator = new QRegExpValidator(QRegExp(".+", Qt::CaseInsensitive, QRegExp::RegExp2), m_ui->edtName);
			m_ui->edtName->setValidator(m_nameValidator);

			m_ui->listWidget->clear();
			m_ui->listWidget->setSelectionMode(QListWidget::SelectionMode::MultiSelection);

			OPENMITTSU_CONNECT(m_ui->listWidget, itemSelectionChanged(), this, onListWidgetItemSelectionChanged());

			// Load all Identities
			auto it = m_knownIdentities.constBegin();
			auto const end = m_knownIdentities.constEnd();

			for (; it != end; ++it) {
				openmittsu::widgets::ContactListWidgetItem* clwi = new openmittsu::widgets::ContactListWidgetItem(it.key(), false, it->nickName);
				m_ui->listWidget->addItem(clwi);
			}
			m_ui->listWidget->clearSelection();
			m_isSelectionOkay = false;

			registerField("edtName*", m_ui->edtName);
			registerField("listWidget*", m_ui->listWidget);
		}

		GroupCreationWizardPageInfo::~GroupCreationWizardPageInfo() {
			delete m_ui;
		}

		bool GroupCreationWizardPageInfo::validatePage() {
			// all data in the Info section was given, now create the group
			QList<QListWidgetItem*> selectedItems = m_ui->listWidget->selectedItems();
			QList<QListWidgetItem*>::const_iterator it = selectedItems.constBegin();
			QList<QListWidgetItem*>::const_iterator end = selectedItems.constEnd();

			QSet<openmittsu::protocol::ContactId> groupMembers;
			for (; it != end; ++it) {
				openmittsu::widgets::ContactListWidgetItem* clwi = dynamic_cast<openmittsu::widgets::ContactListWidgetItem*>(*it);
				groupMembers.insert(clwi->getContactId());
			}

			bool const creationResult = m_groupCreationProcessor->createNewGroup(groupMembers, true, m_ui->edtName->text(), QVariant());
			if (!creationResult) {
				QMessageBox::warning(this, QStringLiteral("Group creation failed!"), QStringLiteral("openMittsu was unable to create and store the new group.\nPlease try again and/or contact a developer!"));
				return false;
			} else {
				return true;
			}
		}

		bool GroupCreationWizardPageInfo::isComplete() const {
			if (m_ui->edtName->hasAcceptableInput() && m_isSelectionOkay) {
				return true;
			}
			return false;
		}

		void GroupCreationWizardPageInfo::onListWidgetItemSelectionChanged() {
			if (m_ui->listWidget->selectedItems().size() > 0) {
				m_isSelectionOkay = true;
			} else {
				m_isSelectionOkay = false;
			}
			emit completeChanged();
		}
	}
}
