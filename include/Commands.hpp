#ifndef _COMMANDS_HPP_INCLUDED_
#define _COMMANDS_HPP_INCLUDED_

//! Receives a CSV file and creates a database based on its contents
/*!
 * Will load the data in the provided CSV file and, one by one, upload them
 * into a database while indexing each entry by id and title using B-trees.
 *
 * Each entry should be represented as a line in the CSV file with the
 * following fields, one in each column:
 *
 * | Field | Type | Description |
 * | --- | --- | --- |
 * | ID | Integer | Identifier of the article |
 * | Title | Alpha 300 | Title of the article |
 * | Year | Integer | Publication year |
 * | Authors | Alpha 1024 | List of the authors |
 * | Citations | Integer | Times the article has been cited |
 * | Update | Date time | Time of the last time the article was updated (`YYYY-MM-DD HH:mm:SS`) |
 * | Snippet | Alpha 1024 | Text summary of the article's contents |
 *
 * Details of the execution will be printed as the program executes.
 *
 * Creates tree files upon completion:
 *
 * - `db-hashfile.bin`: where the entries will be stored;
 * - `db-idindex.bin`: primary index by id;
 * - `db-titleindex.bin`: secondary index by title.
 *
 * The files will be overwritten if they already exist.
 *
 * @param filePath Path to the CSV file with entries
 */
void upload(const char* filePath);

//! Finds an entry in the hashfile based on the entry's id
/*!
 * The hashfile uses perfect hashing. Therefore, the entry's offset in the file
 * can be calculated readily from the id.
 *
 * In case the id doesn't result in a valid offset, the procedure will inform.
 *
 * @param id Id of the entry to find
 */
void findrec(long id);

//! Seeks an entry by its id using the primary index
/*!
 * Uses a B-tree to seek the entry by id within the primary index. In case no
 * entry with the id is found, the procedure will inform.
 *
 * @param id Id of the entry to find
 */
void seek1(long id);

//! Seeks an entry by its title using the secondary index
/*!
 * Uses a B-tree to seek the entry by title within the secondary index.
 *
 * Will return the first entry found with the title.
 *
 * In case no entry with the provided title is found, the procedure will
 * inform.
 *
 * @param title Title of the entry to find
 */
void seek2(const char* title);

#endif
