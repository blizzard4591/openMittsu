#include "src/wizards/ContactEditWizardPageSelectContact.h"
#include "ui_contacteditwizardpageselectcontact.h"

#include "src/exceptions/InternalErrorException.h"

#include "src/utility/MakeUnique.h"

namespace openmittsu {
	namespace wizards {

		ContactEditWizardPageSelectContact::ContactEditWizardPageSelectContact(std::shared_ptr<openmittsu::dataproviders::ContactDataProvider> const& contactDataProvider, QWidget* parent) : QWizardPage(parent), m_ui(std::make_unique<Ui::ContactEditWizardPageSelectContact>()), m_contactDataProvider(contactDataProvider) {
			m_ui->setupUi(this);

			/*nameValidator = new QRegExpValidator(QRegExp(".+", Qt::CaseInsensitive, QRegExp::RegExp2), ui->edtName);
			ui->edtName->setValidator(nameValidator);

			ui->listWidget->clear();
			ui->listWidget->setSelectionMode(QListWidget::SelectionMode::MultiSelection);

			if (!connect(ui->listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onListWidgetItemSelectionChanged()))) {
				qCritical() << QString("Could not connect signal itemSelectionChanged in the GroupCreationWizardPageInfo.");
				throw openmittsu::exceptions::InternalErrorException() << QString("Could not connect signal itemSelectionChanged in the GroupCreationWizardPageInfo.").toStdString();
			}

			// Load all Identities
			ContactRegistry* contactRegistry = ContactRegistry::getInstance();
			QList<quint64> knownIdentities = contactRegistry->getIdentities();
			QList<quint64>::const_iterator it = knownIdentities.constBegin();
			QList<quint64>::const_iterator end = knownIdentities.constEnd();

			quint64 const selfIdentity = (clientConfiguration == nullptr) ? 0 : clientConfiguration->getClientIdentity();
			for (; it != end; ++it) {
				if (*it == selfIdentity) {
					continue;
				}
				ContactListWidgetItem* clwi = new ContactListWidgetItem(contactRegistry->getContact(*it), contactRegistry->getNickname(*it));
				ui->listWidget->addItem(clwi);
			}
			ui->listWidget->clearSelection();
			isSelectionOkay = false;

			registerField("edtName*", ui->edtName);
			registerField("listWidget*", ui->listWidget);*/
		}

		ContactEditWizardPageSelectContact::~ContactEditWizardPageSelectContact() {
			//
		}

		bool ContactEditWizardPageSelectContact::isComplete() const {
			/*if (ui->edtName->hasAcceptableInput() && isSelectionOkay) {
				return true;
			}*/
			return false;
		}

		void ContactEditWizardPageSelectContact::onListWidgetItemSelectionChanged() {
			/*if (ui->listWidget->selectedItems().size() > 0) {
				isSelectionOkay = true;
			} else {
				isSelectionOkay = false;
			}*/
			emit completeChanged();
		}

		QListWidget const* ContactEditWizardPageSelectContact::getSelectedGroupMembersWidgetPointer() const {
			return nullptr;
			//return ui->listWidget;
		}

	}
}
