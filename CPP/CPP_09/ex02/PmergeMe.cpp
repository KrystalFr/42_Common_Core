/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 16:22:13 by krfranco          #+#    #+#             */
/*   Updated: 2026/06/10 18:07:49 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"

// 1 1 3 5 11 21 43 85...
// 1 + (2*0) = 1 + (2*1) = 3 + (2*1) = 5 + (2*3) = 11 + (2*5) = 21 + (2*11) = 43 + (2*21) = 85...
static std::vector<std::size_t> jacobsthal_sequence(std::size_t size)
{
    std::vector<std::size_t> result;
    if (size == 0) return result;

    size_t t0 = 1;
    size_t t1 = 1;
    result.push_back(1);
    result.push_back(1);
    while (result.back() < size)
	{
        size_t tnew = t1 + (2 * t0);
        t0 = t1;
        t1 = tnew;
        result.push_back(tnew);
    }
	
    return result;
}


static std::vector<std::size_t> jacobsthal_order(std::size_t size)
{
    std::vector<std::size_t> order;
    if (size == 0) return order;
    std::vector<bool> used(size, false);

	//recup la suite de jacobsthal jusqu'à size
    std::vector<std::size_t> seq = jacobsthal_sequence(size);
    for (std::size_t i = 0; i < seq.size(); ++i)
	{
        if (seq[i] >= 1 && seq[i] <= size && !used[seq[i] - 1])
		{
            order.push_back(seq[i] - 1);
            used[seq[i] - 1] = true;
        }
    }
	
    for (std::size_t i = 0; i < size; ++i)
	{
        if (!used[i]) order.push_back(i);
    }
	
    return order;
}

std::vector<int> ford_johnson_sort(const std::vector<int> &input)
{
    if (input.size() <= 1) return input;

    std::vector<int> big;
    std::vector<int> small;
    std::vector<int>::const_iterator it = input.begin();

    while (it != input.end())
	{
        int first = *it;
        ++it;
		//si impaire
        if (it == input.end())
		{
            big.push_back(first);
            break;
        }
        int second = *it;
        ++it;
        if (first > second)
		{
            big.push_back(first);
            small.push_back(second);
        } 
		else 
		{
            big.push_back(second);
            small.push_back(first);
        }
    }

    big = ford_johnson_sort(big);

    std::vector<std::size_t> order = jacobsthal_order(small.size());
    for (std::size_t i = 0; i < order.size(); ++i)
	{
        std::size_t idx = order[i];
        int val = small[idx];
// cherche la première position dans big où val peut être inséré sans casser l’ordre croissant
// Avec lower_bound, la recherche est dichotomique:
// on coupe la zone de recherche en deux, on compare seulement avec un élément du milieu, puis on continue dans la moitié gauche ou droite
        std::vector<int>::iterator pos = std::lower_bound(big.begin(), big.end(), val);
        big.insert(pos, val);
    }

    return big;
}

std::deque<int> ford_johnson_sort(const std::deque<int> &input)
{
    if (input.size() <= 1) return input;

    std::deque<int> big;
    std::deque<int> small;
    std::deque<int>::const_iterator it = input.begin();

    while (it != input.end())
	{
        int first = *it;
        ++it;
		//si impaire
        if (it == input.end())
		{
            big.push_back(first);
            break;
        }
        int second = *it;
        ++it;
        if (first > second)
		{
            big.push_back(first);
            small.push_back(second);
        }
		else
		{
            big.push_back(second);
            small.push_back(first);
        }
    }

    big = ford_johnson_sort(big);

    std::vector<std::size_t> order = jacobsthal_order(small.size());
    for (std::size_t i = 0; i < order.size(); ++i)
	{
        std::size_t idx = order[i];
        int val = small[idx];
        std::deque<int>::iterator pos = std::lower_bound(big.begin(), big.end(), val);
        big.insert(pos, val);
    }

    return big;
}