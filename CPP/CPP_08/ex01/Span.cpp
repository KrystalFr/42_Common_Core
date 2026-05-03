/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Span.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 16:50:22 by krfranco          #+#    #+#             */
/*   Updated: 2026/05/03 14:47:25 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Span.hpp"

Span::Span(){}
Span::Span(unsigned int n) : max(n){}
Span::Span(const Span &other) : max(other.max), v(other.v){}
Span::~Span(){}
Span &Span::operator=(const Span &other)
{
	if (this != &other)
	{
		max = other.max;
		v = other.v;
	}
	return *this;
}

void Span::addNumber(int n)
{
	if (v.size() < max)
		v.push_back(n);
	else
		throw FullSpanException();
}

int Span::shortestSpan()
{
	if (v.size() < 2)
		throw NoSpanException();

	int min = longestSpan();
	std::vector<int> tmp = v;
	
	std::sort(tmp.begin(), tmp.end());
	std::vector<int>::iterator itPrev = tmp.begin();
	std::vector<int>::iterator itCur = itPrev;

	for (++itCur; itCur != tmp.end(); ++itCur)
	{
		if (*itCur - *itPrev < min)
			min = *itCur - *itPrev;
		itPrev = itCur;
	}
	return min;

}

int Span::longestSpan()
{
	if (v.size() < 2)
		throw NoSpanException();

	int span;
	span = *max_element(v.begin(), v.end()) - *min_element(v.begin(), v.end());

	return (span);
}

const char *Span::FullSpanException::what() const throw()
{
	return "Span is full";
}

const char * Span::NoSpanException::what() const throw()
{
	return "No span possible";
}