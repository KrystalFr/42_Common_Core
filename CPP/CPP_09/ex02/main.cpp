/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 16:22:10 by krfranco          #+#    #+#             */
/*   Updated: 2026/06/06 22:49:55 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"
#include <climits>
#include <ctime>
#include <iostream>


template <typename Container>
static void printContainer(const Container &c)
{
	for (typename Container::const_iterator it = c.begin(); it != c.end(); ++it)
		std::cout << *it << " ";
}

static bool validNumber(const char *str, int &nb)
{
	char *end = NULL;
	long value = std::strtol(str, &end, 10);
	if (str[0] == '\0' || end == NULL || *end != '\0')
		return false;
	if (value <= 0 || value > INT_MAX)
		return false;
	nb = static_cast<int>(value);
	return true;
}

int main(int ac, char **av)
{
	std::vector<int> beforeVect;
	std::deque<int> beforeDeq;

	
	if (ac < 2)
	{
		std::cout << "No input provided; auto-testing with 300 descending integers (300..1)." << std::endl;
		for (int i = 300; i >= 1; --i)
		{
			beforeVect.push_back(i);
			beforeDeq.push_back(i);
		}
	}
	else
	{
		for (int i = 1; i < ac; ++i)
		{
			int nb;
			if (!validNumber(av[i], nb))
			{
				std::cerr << "Error" << std::endl;
				return 1;
			}
			beforeVect.push_back(nb);
			beforeDeq.push_back(nb);
		}
	}

	std::cout << "Before: ";
	printContainer(beforeVect);
	std::cout << std::endl;

	
	std::clock_t startVect = std::clock();
	std::vector<int> afterVect = ford_johnson_sort(beforeVect);
	std::clock_t endVect = std::clock();

	std::clock_t startDeq = std::clock();
	std::deque<int> afterDeq = ford_johnson_sort(beforeDeq);
	std::clock_t endDeq = std::clock();

	
	std::cout << "After:  ";
	printContainer(afterVect);
	std::cout << std::endl;

	double vecTime = (static_cast<double>(endVect - startVect) / CLOCKS_PER_SEC) * 1000000.0;
	double deqTime = (static_cast<double>(endDeq - startDeq) / CLOCKS_PER_SEC) * 1000000.0;

	std::cout << "Time to process a range of " << beforeVect.size()
	          << " elements with std::vector : " << vecTime << " us" << std::endl;
	std::cout << "Time to process a range of " << beforeDeq.size()
	          << " elements with std::deque : " << deqTime << " us" << std::endl;

	return 0;
}