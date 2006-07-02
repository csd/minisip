/*
  Copyright (C) 2006 Werner Dittmann 
 
  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the
  Free Software Foundation; either version 2.1 of the License, or (at your
  option) any later version.
 
  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
  for more details.
 
  You should have received a copy of the GNU Lesser General Public License
  along with this library; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

/*
 * Authors: Werner Dittmann <Werner.Dittmann@t-online.de>
 */

#include <stdio.h>
#include <stdint.h>

#include <libminisip/zrtp/ZIDRecord.h>

#ifndef _ZIDFILE_H_
#define _ZIDFILE_H_

/**
 * This class manages a ZID file.
 * The ZID file holds information about peers. 
 *
 * <p/>
 *
 * NOTE: This class is a friend of the ZIDRecord class. Please keep both
 * classes synchronized.
 */

class ZIDFile {

private:

    FILE *zidFile;
    uint8_t associatedZid[IDENTIFIER_LEN];
    /**
     * The private ZID file constructor.
     *
     */
    ZIDFile(): zidFile(NULL) {};
    ~ZIDFile();

public:

    /**
     * Get the an instance of ZIDFile.
     *
     * This method just creates an instance an store a pointer to it
     * in a static variable. The ZIDFile is a singleton, thus only
     * <e>one</e> ZID file can be open at one time.
     *
     * @return
     *    A pointer to the global ZIDFile singleton instance.
     */
    static ZIDFile *getInstance();
    /**
     * Open the named ZID file and return a ZID file class.
     *
     * This static function either opens an existing ZID file or
     * creates a new ZID file with the given name. The ZIDFile is a
     * singleton, thus only <e>one</e> ZID file can be open at one
     * time.
     *
     * <p/>
     *
     * To open another ZID file you must close the active ZID file 
     * first.
     * 
     * @param name 
     *    The name of the ZID file to open or create
     * @return 
     *    1 if file could be opened/created, 0 if the ZID instance 
     *    already has an open file, -1 if open/creation of file failed.
     */
    int32_t open(char *name);

    /**
     * Check if ZIDFile has an active (open) file.
     *
     * @return
     *    1 if ZIDFile has an active file, 0 otherwise
     */
     int32_t isOpen() { return (zidFile == NULL); };

     /**
     * Close the ZID file.
     * Closes the ZID file, and prepares to open a new ZID file.
     */
    void close();

    /**
     * Get a ZID record from the active ZID file.
     *
     * The method get the identifier data from the ZID record parameter,
     * locates the record in the ZID file and fills in the RS1 and RS2
     * data. If no matching record exists in the ZID file the method creates
     * it and fills it with default values.
     * 
     * @param zidRecord
     *    The ZID record that contains the identifier data. The method 
     *    fills in the RS1 and RS2 data.
     * @return
     *    TODO
     */
    uint32_t getRecord(ZIDRecord *zidRecord);

    /**
     * Save a ZID record into the active ZID file.
     *
     * This method saves the content of a ZID record into the ZID file. Before
     * you can save the ZID record you must have performed a getRecord()
     * first.
     *
     * @param zidRecord
     *    The ZID record to save.
     * @return
     *    1 on success, 0 on failure
     */
    uint32_t saveRecord(ZIDRecord *zidRecord);
    
    /**
     * Get the ZID associated with this ZID file.
     *
     * @return
     *    Pointer to the ZID
     */
    const uint8_t* getZid() { return associatedZid; };
};
#endif

