#ifndef _ENTRY_HPP_INCLUDED_
#define _ENTRY_HPP_INCLUDED_

#define TITLE_CHAR_MAX 300
#define AUTHORS_CHAR_MAX 1024
#define TIMESTAMP_CHAR_MAX 20
#define SNIPPET_CHAR_MAX 1024

struct Entry {
	bool valid;
	int id;
	char title[TITLE_CHAR_MAX];
	int year;
	char authors[AUTHORS_CHAR_MAX];
	int citations;
	char updateTimestamp[TIMESTAMP_CHAR_MAX];
	char snippet[SNIPPET_CHAR_MAX];
};

#endif // _ENTRY_HPP_INCLUDED_
