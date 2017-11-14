#include "src/widgets/LicenseDialog.h"
#include "ui_LicenseDialog.h"

#include <QIcon>
#include <QSize>

LicenseDialog::LicenseDialog(QWidget *parent) : QDialog(parent), ui(new Ui::LicenseDialog) {
    ui->setupUi(this);

	this->setAttribute(Qt::WA_DeleteOnClose);
	QIcon icon = this->windowIcon();
	QSize size = icon.actualSize(QSize(64, 64));
	ui->lblIcon->setPixmap(icon.pixmap(size));

	QString const licenseText = QString("<h2>OpenMittsu</h2><br><br>")
		.append(tr("An open source chat client for Threema-style end-to-end encrypted chat networks."))
		.append(QStringLiteral("<br><br>"))
		.append(tr("Copyright (C) 2015-16 <a href=\"https://www.philippberger.de/\">Philipp Berger</a>"))
		.append(QStringLiteral("<br>"))
		.append(tr("This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version."))
		.append(QStringLiteral("<br><br>"))
		.append(tr("This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the GNU General Public License for more details."))
		.append(QStringLiteral("<br><br>"))
		.append(tr("You should have received a copy of the GNU General Public License along with this program. If not, see <a href=\"http://www.gnu.org/licenses/\">http://www.gnu.org/licenses/</a>."))
		.append(QStringLiteral("<br><br><br>"))
		.append(tr("This program incorporates work covered by the following copyright and permission notices: "))
		.append(QStringLiteral("<br><ul><li>"))
		.append(tr("The DejaVu fonts. Copyright by Bitstream. DejaVu changes are in public domain. Explanation of copyright is on Gnome page on Bitstream Vera fonts. Glyphs imported from Arev fonts are Copyright by Tavmjung Bah.	See <a href=\"http://dejavu-fonts.org/wiki/License\">http://dejavu-fonts.org/wiki/License</a> for more information."))
		.append(QStringLiteral("</li><li>"))
		.append(tr("The Source Sans Pro fonts. Copyright 2010, 2012, 2014 Adobe Systems Incorporated (<a href=\"http://www.adobe.com/\">http://www.adobe.com/</a>), with Reserved Font Name 'Source'. All Rights Reserved. Source is a trademark of Adobe Systems Incorporated in the United States and/or other countries. This Font Software is licensed under the SIL Open Font License, Version 1.1."))
		.append(QStringLiteral("</li><li>"))
		.append(tr("The OpenSansEmoji font. Licensed under the Apache License, Version 2.0 (the \"License\"); you may not use this file except in compliance with the License."))
		.append(QStringLiteral("</li><li>"))
		.append(tr("Sound \"NewMessage.mp3\", from <a href=\"https://freesound.org/people/melliug/sounds/221359/\">https://freesound.org/people/melliug/sounds/221359/</a>. Licensed under the Creative Commons 0 License. See <a href=\"http://creativecommons.org/publicdomain/zero/1.0/\">http://creativecommons.org/publicdomain/zero/1.0/</a>."))
		.append(QStringLiteral("</li><li>"))
		.append(tr("LibQREncode, linked either statically or dynamically in most releases. Copyright (C) 2006-2012 Kentaro Fukuchi (<a href=\"http://fukuchi.org/works/qrencode/\">http://fukuchi.org/works/qrencode/</a>). Licensed under the GNU Lesser General Public License, version 2.1."))
		.append(QStringLiteral("</li><li>"))
		.append(tr("LibSodium, linked either statically or dynamically in most releases. Copyright(c) 2013 - 2015 Frank Denis (j at pureftpd dot org). Licensed under the ISC license."))
		.append(QStringLiteral("</li><li>"))
		.append(tr("SpdLog (<a href=\"https://github.com/gabime/spdlog\">https://github.com/gabime/spdlog</a>). Licensed under the The MIT License (MIT). Copyright(c) 2015 Gabi Melman. Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software."))
		.append(QStringLiteral("</li></ul><br><br>"));
	ui->lblLicenseText->setText(licenseText);
}

LicenseDialog::~LicenseDialog() {
    delete ui;
}
