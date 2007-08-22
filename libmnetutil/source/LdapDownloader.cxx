/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 */

#include <config.h>
#include <libmnetutil/LdapDownloader.h>

#include <libmnetutil/LdapUrl.h>
#include <libmnetutil/LdapCredentials.h>

#include <fstream>

LdapDownloader::LdapDownloader(std::string originalUrl) : isLoaded(false) {
	url = LdapUrl(originalUrl);
	if (url.isValid()) {
		conn = MRef<LdapConnection*>(new LdapConnection(url.getHost(), url.getPort(), MRef<LdapCredentials*>(new LdapCredentials("", ""))));
	}
}
void LdapDownloader::fetch() {
	if (url.isValid()) {
		try {
			std::vector<std::string> attrs = url.getAttributes();
			entries = conn->find(url.getDn(), url.getFilter(), attrs, url.getScope());
			isLoaded = true;
		} catch (LdapNotConnectedException & /*e*/) {
		} catch (LdapException & /*e*/) {
		}
	}
}
char* LdapDownloader::getChars(int *length) {
	if (!isLoaded)
		fetch();
	if (isLoaded) {
		if (entries.size() > 0) {
			MRef<LdapEntry*> entry = entries.at(0);
			std::vector<std::string> attrs = entry->getAttrNames();
			if (attrs.size() > 0) {
				/*
				 * First try if the attribute is a string attribute.
				 */
				try {
					std::string firstAttrValue = entry->getAttrValueString(attrs.at(0));

					*length = (int)firstAttrValue.length();
					char* res = new char[*length];
					memcpy(res, firstAttrValue.c_str(), *length);
					return res;
				} catch (LdapAttributeNotFoundException & /*e*/) {
					/*
					 * Could not find the first named attribute amongst the string
					 * attributes. Try to find the same attribute name in the
					 * collection of binary attributes.
					 */
					try {
						std::vector< MRef<LdapEntryBinaryValue*> > firstAttrValues = entry->getAttrValuesBinary(attrs.at(0));
						if (firstAttrValues.size() > 0) {
							*length = firstAttrValues.at(0)->length;
							char* res = new char[*length];
							memcpy(res, firstAttrValues.at(0)->value, *length);
							return res;
						}
					} catch (LdapAttributeNotFoundException & /*e*/) {
						/*
						 * Could not find attribute in collection of binary attributes
						 * either. This is very odd and should be be able to happen!
						 */
					}
				}
			}
		}
	}
	*length = 0;
	return NULL;
}

std::vector<std::string> LdapDownloader::saveToFiles(std::string attr, std::string filenameBase, bool onlyFirst) {
	std::vector<std::string> filenames;

	if (!isLoaded)
		fetch();
	if (isLoaded) {
		if (entries.size() > 0) {
			MRef<LdapEntry*> entry = entries.at(0);
			try {
				std::vector< MRef<LdapEntryBinaryValue*> > binaryData = entry->getAttrValuesBinary(attr);

				for (size_t i=0; i<binaryData.size(); i++) {
					std::string fileName = nextFilename(filenameBase, (int)i+1);
					std::ofstream file(fileName.c_str());
					if (file.good()) {
						MRef<LdapEntryBinaryValue*> val = binaryData.at(i);
						file.write(val->value, val->length);
						file.close();
						filenames.push_back(fileName);
					}
					if (onlyFirst && i==0)
						break;
				}
			} catch (LdapAttributeNotFoundException & /*e*/) {

			}
		}
	}
	return filenames;
}
std::string LdapDownloader::nextFilename(std::string baseName, int num) {
	std::string newName = baseName;
	size_t pos = 0;
	if (1 < num) {
		pos = baseName.find_last_of('.', baseName.length());
		if (pos == std::string::npos) {
			newName = baseName + "." + itoa(num);
		} else {
			newName = baseName.substr(0, pos) + "." + itoa(num) + baseName.substr(pos);
		}
	}
	return newName;
}


std::string LdapDownloader::getString(std::string attr) throw (LdapAttributeNotFoundException, LdapException) {
	if (!isLoaded)
		fetch();
	if (isLoaded) {
		if (entries.size() > 0) {
			MRef<LdapEntry*> entry = entries.at(0);
			try {
				return entry->getAttrValueString(attr);
			} catch (LdapAttributeNotFoundException & /*e*/) {
				throw;
			}
		}
	}
	throw LdapException("No entry fetched");
}

MRef<LdapEntryBinaryValue*> LdapDownloader::getBinary(std::string attr) throw (LdapAttributeNotFoundException, LdapException) {
	if (!isLoaded)
		fetch();
	if (isLoaded) {
		if (entries.size() > 0) {
			MRef<LdapEntry*> entry = entries.at(0);
			try {
				std::vector< MRef<LdapEntryBinaryValue*> > vals = entry->getAttrValuesBinary(attr);
				if (vals.size() > 0) {
					return vals.at(0);
				}
			} catch (LdapAttributeNotFoundException & /*e*/) {
				throw;
			}
		}
	}
	throw LdapException("No entry fetched");
}
