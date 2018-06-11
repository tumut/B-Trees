#ifndef _ENTRY_HPP_INCLUDED_
#define _ENTRY_HPP_INCLUDED_

//! Max title size in characters
#define TITLE_CHAR_MAX 300

//! Max author name list size in total characters
#define AUTHORS_CHAR_MAX 1024

//! Max timestamp size in characters
#define TIMESTAMP_CHAR_MAX 20

//! Max snippet size in characters
#define SNIPPET_CHAR_MAX 1024

//! Article entry
/*!
 * Basic file entry
 */
struct Entry {
	bool valid; //!< False if the entry being used only for padding within the file, false if the data is valid
	int id; //!< Article identifier
	char title[TITLE_CHAR_MAX]; //!< Article title
	int year; //!< Publication year of the article
	char authors[AUTHORS_CHAR_MAX]; //!< Article authors
	int citations; //!< Times the article has been cited
	char updateTimestamp[TIMESTAMP_CHAR_MAX]; //!< Last update timestamp
	char snippet[SNIPPET_CHAR_MAX]; //!< Article snippet
};

#endif // _ENTRY_HPP_INCLUDED_
