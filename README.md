# BTrees

B-Tree implementation in C++ to index documents based on numerical id's (main index) and title strings (secondary index). Created as a college assignment.

[Click here for the code documentation.](https://tumut.github.io/BTrees/) (Pre-generated documentation currently unavailable.)

## Building

The project was developed in `CLion` and can be built from there, though you can also use `CMake` for it.

## Input

The database that was used for testing and development can be downloaded [here](https://drive.google.com/file/d/0B5H52GfdcU-WLVJHOWR3UzB0YzQ/view).

The program was built to accept a `;`-separated CSV file. Each line represents an article, and the columns reflect the following fields (type _Alpha N_ meaning "a string with at most N alphanumeric characters"):

| Field | Type | Description |
| --- | --- | --- |
| ID | Integer | Identifier of the article |
| Title | Alpha 300 | Title of the article |
| Year | Integer | Publication year |
| Authors | Alpha 1024 | List of the authors |
| Citations | Integer | Times the article has been cited |
| Update | Date time | Time of the last time the article was updated (`YYYY-MM-DD HH:mm:SS`) |
| Snippet | Alpha 1024 | Text summary of the article's contents |

## Usage

To do.
