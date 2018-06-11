# 🅱️ Trees

B-tree implementation in C++ to index documents based on numerical id's (main index) and title strings (secondary index) in a sample application. Created as a college assignment.

## Building

The project can be opened in `CLion` or compiled with `CMake`.

The documentation was generated with `doxygen` and the repository's `Doxyfile`. Run `doxygen` from the repository's root folder or check [the pre-generated documentation](https://tumut.github.io/BTrees/).

## Input

The database that was used for testing and development can be downloaded [here](https://drive.google.com/file/d/0B5H52GfdcU-WLVJHOWR3UzB0YzQ/view).

The program was built to accept a `;`-separated CSV file with quote-enclosed values. Each line should have the following fields (type _Alpha N_ meaning "a string with at most N alphanumeric characters"):

| Field | Type | Description |
| --- | --- | --- |
| ID | Integer | Identifier of the article |
| Title | Alpha 300 | Title of the article |
| Year | Integer | Publication year |
| Authors | Alpha 1024 | List of the authors |
| Citations | Integer | Times the article has been cited |
| Update | Date time | Time of the last time the article was updated (`YYYY-MM-DD HH:mm:SS`) |
| Snippet | Alpha 1024 | Text summary of the article's contents |

Example line:

`"1";"Title";"2017";"Alice,Bob,Carl";"42";"2017-01-30 23:59:59";"Snippet"`

Fields can be left blank.

## Usage

Use either of the following commands with the executable:

* `$ <exec-name> upload <input>`
	
	Upload a CSV file `input` with entries into the database.

	Will generate three files: one for the primary index (`db-idindex.bin`), another for the secondary index (`db-titleindex.bin`), and finally one where the entries themselves will be stored (`db-hashfile.bin`). 

	The files will be overwritten if they already exist.

* `$ <exec-name> findrec <hashfile-id>`

	Find an entry by its numeric index `id`, by looking directly at the hashfile.

* `$ <exec-name> seek1 <id>`

	Find an entry by its numeric index `id`, by seeking its hashfile location in the primary index.

* `$ <exec-name> seek2 <title>`

	Find an entry by its title `title`, by seeking its hashfile location in the secondary index. The title must be an exact match.
