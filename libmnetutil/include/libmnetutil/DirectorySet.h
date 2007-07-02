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
 *              Erik Eliasson <eliasson@it.kth.se>
 *          	Johan Bilien <jobi@via.ecp.fr>
 *          	Mikael Magnusson <mikma@users.sourceforge.net>
 */

#ifndef _DIRECTORYITEM_H_
#define _DIRECTORYITEM_H_

#include <libmnetutil/libmnetutil_config.h>
#include <libmutil/MemObject.h>
#include <libmutil/Mutex.h>

#include <libmnetutil/DirectorySetItem.h>

/**
 * Class for managing a set of directory service information items.
 *
 * More or less a duplicate of the CertificateSet class.
 *
 * Sample code:
 * @code
#include <libmnetutil/DirectorySet.h>
#include <libmnetutil/DirectorySetItem.h>
#include <string>
#include <list>
#include <iostream>

int main (int argc, char* argv[]) {
	std::list<MRef<DirectorySetItem *> > items;
	std::list<MRef<DirectorySetItem *> >::iterator allItemsIter;

	DirectorySet* set = DirectorySet::create();

	//CREATING THE SET
	set->addLdap("ldap://ucd-slow.minisip.org/dc=ucd-slow,dc=minisip,dc=org", "ucd-slow.minisip.org");
	set->addLdap("ldap://ucd-slow.minisip.org/dc=ucd-slow,dc=minisip,dc=org", "minisip.org");
	set->addLdap("ldap://ucd-fast.ssvl.kth.se/dc=ucd-fast,dc=ssvl,dc=kth,dc=se", "ucd-fast.ssvl.kth.se");

	//PRINTING THE COMPLETE SET
	std::cout << "The complete set:" << std::endl;
	for (allItemsIter = set->getItems().begin(); allItemsIter != set->getItems().end(); allItemsIter++) {
		std::cout << "   " << (*allItemsIter)->getSubTree() << " is handled by " << (*allItemsIter)->getUrl() << std::endl;
	}
	std::cout << std::endl;


	//SEARCHING THE SET
	std::string patterns[] = {"kth.se", 	"minisip.org",	"minisip.org",	"ucd-fast.minisip.org",	"sunet.se"};
	bool partialMatching[] = {true, 	false, 		true, 		true, 			false};
	int num = 5;

	for (int i=0; i < num; i++) {
		std::cout << "Searching for directories responsible for " << patterns[i] << (partialMatching[i] ? " and sub-domains" : " but not its sub-domains") << ":" << std::endl;

		items = set->findItems(patterns[i], partialMatching[i]);
		while (!items.empty()) {
			std::cout << "   Match: " << items.front()->getSubTree() << " URL: " << items.front()->getUrl() << std::endl;
			items.pop_front();
		}
		std::cout << std::endl;
	}


	delete set;

	return 0;
}
 * @endcode
 * @author	Mikael Svensson
 */
class LIBMNETUTIL_API DirectorySet : public MObject {
	public:
		DirectorySet();
		~DirectorySet();
		static DirectorySet *create();

		DirectorySet* clone();
		void addLdap (const std::string url, const std::string subTree);

		/**
		 * Returns \em reference to the list of all directory items in the set
		 *
		 * @note	This function resets the internal list iterator.
		 */
		std::list<MRef<DirectorySetItem*> > & getItems();

		/**
		 * Returns a list of directory items matching the given sub-tree identifier.
		 *
		 * The second parameter sets if the whole sub-tree string should be matched or only the last
		 * part of. This is useful if one directory item has subTree = "kth.se" and another one has
		 * subTree = "ssvl.kth.se" and you want to retrieve both of them: then you just set the
		 * first function parameter to "kth.se" and the second to "true".
		 *
		 * @deprecated	I see no use for this function. findItemsPrioritized() provides a more useful functionality.
		 * @note	This function does \em not return a reference variable, which getItems() do.
		 * 		However, even though it returns a new list, each of the list items still refer
		 * 		to the exact same items that are stored in the set (i.e. items are not cloned
		 * 		before returned by this function).
		 * @note	This function resets the internal list iterator.
		 * @param	subTree			The string to search for in the \c subTree property of
		 * 					each DirectorySetItem object in the set.
		 * @param	endsWithIsEnough	If true (default): function returns all DirectorySetItem
		 * 					objects where the subTree string ends with \p subTree.
		 * 					If false: function returns only items where there is and
		 * 					exact matching of \p subTree.
		 */
		std::list<MRef<DirectorySetItem*> > findItems(const std::string subTree, const bool endsWithIsEnough = true);

		/**
		 * Returns a list of directory items where the first item in the list represents the
		 * directory most likely to maintain information about users in \p domain.
		 *
		 * The function works like this:
		 * - First and foremost all directory items are scanned and those that have some
		 *   part of their domain name in common with \p domain make it to the next step.
		 *
		 *   A directory URL \c i have "something in common" with \p domain if the string
		 *   \p domain ends with the string \c i. This means that if we are looking for
		 *   directories with information about alice@ssvl.kth.se and there is a cached
		 *   directory item for "kth.se", then that cached item should be returned. If,
		 *   however, we are looking for "alice@kth.se" it would not be OK to return a
		 *   cached directory item for "ssvl.kth.se".
		 *
		 * - The second phase is simply a sorting of the result from phase 1: the longest
		 *   match should be at the beginning of the list ("ssvl.kth.se" outranks "kth.se"
		 *   as it is more probable that the former has information about "alice@ssvl.kth.se"
		 *   than the latter).
		 */
		std::vector<MRef<DirectorySetItem*> > findItemsPrioritized(const std::string domain);

		MRef<DirectorySetItem*> getNext();

		void initIndex();
		void lock();
		void unlock();

		void remove (MRef<DirectorySetItem*> removedItem);

		void addItem (const MRef<DirectorySetItem*> item);

		/**
		 * Create directory item refering to an LDAP directory
		 */
		MRef<DirectorySetItem*> createItemLdap(const std::string url, const std::string subTree);

	private:
		std::list<MRef<DirectorySetItem*> >::iterator itemsIndex;
		std::list<MRef<DirectorySetItem*> > items;
		Mutex mLock;
};
#endif
