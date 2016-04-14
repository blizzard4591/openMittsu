#include "tasks/CallbackTask.h"

#include "utility/Logging.h"

CallbackTask::CallbackTask() : QThread(nullptr), cb_errorCode(0), cb_errorMessage(""), cb_isFinished(false), cb_finishedSuccessfully(false) {
	// Intentionally left empty.
}

CallbackTask::~CallbackTask() {
	// Intentionally left empty.
}

int CallbackTask::getErrorCode() const {
	return cb_errorCode;
}

QString const & CallbackTask::getErrorMessage() const {
	return cb_errorMessage;
}

void CallbackTask::preRunSetup() {
	// This is a default implementation which does not do anything.
}

void CallbackTask::taskRun() {
	// This is a default implementation which does not do anything.
	finishedWithNoError();
}

void CallbackTask::run() {
	LOGGER_DEBUG("CallbackTask::run() will now run the setup.");
	preRunSetup();
	LOGGER_DEBUG("CallbackTask::run() will now enter the taskRun section.");
	taskRun();
	LOGGER_DEBUG("CallbackTask::run() has left the taskRun section.");

	this->exit(0);
}

void CallbackTask::finishedWithNoError() {
	// The Error Code and Message are in the success state by default.
	this->cb_isFinished = true;
	this->cb_finishedSuccessfully = true;

	emit finished(this);
}

void CallbackTask::finishedWithError(int errorCode, QString const & errorMessage) {
	this->cb_errorCode = errorCode;
	this->cb_errorMessage = errorMessage;
	this->cb_isFinished = true;
	this->cb_finishedSuccessfully = false;

	emit finished(this);
}

bool CallbackTask::hasFinished() const {
	return cb_isFinished;
}

bool CallbackTask::hasFinishedSuccessfully() const {
	return cb_finishedSuccessfully;
}
