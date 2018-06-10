#ifndef _BLOCK_HPP_INCLUDED_
#define _BLOCK_HPP_INCLUDED_

//! Block size in bytes.
/*!
	The size was defined as 4096 bytes based on sector storage size of the
	local machine, obtained through an `fdisk -l` command.
*/
#define BLOCK_SIZE 4096

//! Union for reading and writing blocks containing serialized data.
/*!
	T must be a POD (Plain Old Data type) and will be the type of the
	serialized data within the block.

	The condition `sizeof(T) <= BLOCK_SIZE` must be satisfied. Otherwise, the
	stored value will be bigger than the block size and reading and writing
	will be compromised.

	This union is used to guarantee that written data in the files will always
	individually occupy exactly one block in size. The remaining unused bytes
	will have undefined values in the file.

	After using Block<T> instances to write a file, you should use it again
	when reading the file.
*/
template <class T>
union Block {
	char padding[BLOCK_SIZE]; //!< Byte array used to guarantee that the instance will have at least `BLOCK_SIZE` bytes.
	T var; //!< Member used to access the wrapped value.
};

#endif // _BLOCK_HPP_INCLUDED_

/** @file */
