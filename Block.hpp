#ifndef _BLOCK_HPP_INCLUDED_
#define _BLOCK_HPP_INCLUDED_

#define BLOCK_SIZE 4096

template <class T>
union Block {
	char padding[BLOCK_SIZE];
	T var;
};

#endif // _BLOCK_HPP_INCLUDED_
