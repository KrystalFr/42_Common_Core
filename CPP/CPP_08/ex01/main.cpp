/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 17:12:14 by krfranco          #+#    #+#             */
/*   Updated: 2026/05/03 14:55:47 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Span.hpp"

int main()
{
	Span sp = Span(5);
	sp.addNumber(6);
	sp.addNumber(3);
	sp.addNumber(17);
	sp.addNumber(9);
	sp.addNumber(11);
	std::cout << "Shortest span: " << sp.shortestSpan() << std::endl;
	std::cout << "Longest span: " << sp.longestSpan() << std::endl;

	std::cout << std::endl;

	// Test with range iterator + large num
	std::vector<int> range;
    for (int i = 0; i < 10000; i++)
		range.push_back(i);

	Span sp2(10000);
    sp2.addNumber(range.begin(), range.end());
	
    std::cout << "Shortest span in 10000 numbers: " << sp2.shortestSpan() << std::endl;
    std::cout << "Longest span in 10000 numbers: " << sp2.longestSpan() << std::endl;
	return 0;
}