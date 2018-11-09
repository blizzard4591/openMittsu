#ifndef OPENMITTSU_WIZARDS_FIRSTUSEWIZARD_H_
#define OPENMITTSU_WIZARDS_FIRSTUSEWIZARD_H_

#include <QWizard>
#include <memory>

namespace Ui {
	class FirstUseWizard;
}

namespace openmittsu {
	namespace wizards {

		class FirstUseWizard : public QWizard {
			Q_OBJECT
		public:
			explicit FirstUseWizard(QWidget* parent = nullptr);
			virtual ~FirstUseWizard();

			enum UserChoice {
				INVALID,
				LOAD_DATABASE,
				CREATE_DATABASE
			};
			UserChoice getUserChoice() const;
		private slots:
			void onButtonPressed(int id);
		private:
			std::unique_ptr<Ui::FirstUseWizard> m_ui;
			UserChoice m_userChoice;
		};

	}
}

#endif // OPENMITTSU_WIZARDS_FIRSTUSEWIZARD_H_
