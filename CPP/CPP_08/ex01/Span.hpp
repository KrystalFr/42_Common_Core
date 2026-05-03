/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Span.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 16:50:24 by krfranco          #+#    #+#             */
/*   Updated: 2026/05/03 14:42:12 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SPAN_HPP
#define SPAN_HPP

#include <iostream>
#include <exception>
#include <vector>
#include <algorithm>
#include <iterator>

class Span
{
	private:
		unsigned int max;
		std::vector<int> v;
		
	public:
		Span();
		Span(unsigned int n);
		Span(const Span &other);
		~Span();
		Span &operator=(const Span &other);

		void addNumber(int n);

		template<typename Iterator>
		void addNumber(Iterator begin, Iterator end)
		{
			for (Iterator it = begin; it != end; ++it)
				addNumber(*it);
		}
		
		int shortestSpan();
		int longestSpan();

		class FullSpanException : public std::exception
		{
			public:
				const char *what() const throw();
		};

		class NoSpanException : public std::exception
		{
			public:
				const char *what() const throw();
		};
};
#endif