/*
  Copyright (C) 2007, Mikael Svensson

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
 * Authors:	Mikael Svensson
*/

#include <config.h>
#include <libmnetutil/DirectorySetItem.h>

DirectorySetItem::DirectorySetItem() : type(DIRECTORYSETITEM_TYPE_LDAP), url(""), subTree("") {}

DirectorySetItem::DirectorySetItem(std::string url, std::string subTree) : type(DIRECTORYSETITEM_TYPE_LDAP), url(url), subTree(subTree) {}

DirectorySetItem::DirectorySetItem(int32_t type, std::string url, std::string subTree) : type(type), url(url), subTree(subTree) {}

DirectorySetItem::DirectorySetItem(int32_t type) : type(type), url(""), subTree("") {}

bool DirectorySetItem::operator ==(const DirectorySetItem item2) const {
	return (
		item2.getUrl() == url &&
		item2.getType() == type &&
		item2.getSubTree() == subTree
	       );
}

int32_t DirectorySetItem::getType() const {
	return type;
}
std::string DirectorySetItem::getUrl() const {
	return url;
}
std::string DirectorySetItem::getSubTree() const {
	return subTree;
}

void DirectorySetItem::setType(const int32_t type) {
	this->type = type;
}
void DirectorySetItem::setUrl(const std::string url) {
	this->url = url;
}
void DirectorySetItem::setSubTree(const std::string subTree) {
	this->subTree = subTree;
}
