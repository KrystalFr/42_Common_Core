/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 16:22:16 by krfranco          #+#    #+#             */
/*   Updated: 2026/06/06 22:47:01 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <algorithm>
#include <deque>
#include <vector>
#include <cstddef>
#include <iterator>

// Concrete functions for the exercise: vector<int> and deque<int>
std::vector<int> ford_johnson_sort(const std::vector<int> &src);
std::deque<int>  ford_johnson_sort(const std::deque<int> &src);

#endif