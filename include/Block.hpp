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
	Wraps a T value within a block-sized memory space.

	The condition `sizeof(T) <= BLOCK_SIZE` must be satisfied. Otherwise, the
	stored value will be bigger than the block size and reading and writing
	will be compromised.

	This union is used to guarantee that written data in the files will always
	individually occupy exactly one block in size. The remaining unused bytes
	will have undefined values in the file.

	After using Block instances to write a file, use Block again when reading
	the file.

	@tparam T Type of the data to be stored within the block. Most be a POD
	(Plain Old Data type).
*/
template <class T>
union Block {
	char padding[BLOCK_SIZE]; //!< Byte array used to guarantee that the instance will have at least `BLOCK_SIZE` bytes.
	T var; //!< Member used to access the wrapped value.
};

#endif // _BLOCK_HPP_INCLUDED_

/** @file */
