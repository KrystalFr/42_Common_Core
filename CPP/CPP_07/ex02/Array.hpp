/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Array.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 19:19:13 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/09 13:55:37 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <iostream>
#include <exception>

template <typename T>
class Array
{
	private:
		T *array;
		unsigned int len;
	
	public:
		Array();
		Array(const unsigned int len);
		Array(const Array &other);
		Array &operator=(const Array &other);
		~Array();

		unsigned int size() const;
		T &operator[](unsigned int i);
		T const &operator[](unsigned int i) const;

		class OutOfBounds : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
};

#include "Array.tpp"
#endif