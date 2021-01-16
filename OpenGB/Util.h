#pragma once

#define test_bit(t, pos)		\
	(t & (1 << pos))

#define get_bit(t, pos)			\
	(test_bit(t, pos) ? 1 : 0)

#define set_bit(t, pos)			\
	t = t | (1 << pos)

#define flip_bit(t, pos)		\
	if (test_bit(t, pos)) {		\
		clear_bit(t, pos);		\
	} else {					\
		set_bit(t, pos);		\
	}

#define clear_bit(t, pos)		\
	t = t & ~(1 << pos)