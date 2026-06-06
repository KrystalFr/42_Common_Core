/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 16:22:13 by krfranco          #+#    #+#             */
/*   Updated: 2026/06/06 22:59:28 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"

// --- Jacobsthal helpers (book variant: sequence starting 1,1,3,5,11,21...) ---
static std::vector<std::size_t> jacobsthal_sequence_up_to(std::size_t m)
{
    std::vector<std::size_t> seq;
    if (m == 0) return seq;
    // book-like: t0 = 1, t1 = 1, t_k = t_{k-1} + 2 * t_{k-2}
    unsigned long t0 = 1;
    unsigned long t1 = 1;
    seq.push_back(1); // t0
    seq.push_back(1); // t1 (duplicate, convenient)
    while (seq.back() < m) {
        unsigned long tn = t1 + 2 * t0;
        t0 = t1;
        t1 = tn;
        seq.push_back(static_cast<std::size_t>(tn));
    }
    return seq;
}

// Build an insertion order of indices [0..m-1] based on Jacobsthal boundaries.
// First we push indices j-1 for each Jacobsthal j in range, then remaining indices ascending.
static std::vector<std::size_t> jacobsthal_order(std::size_t m)
{
    std::vector<std::size_t> order;
    if (m == 0) return order;
    std::vector<bool> used(m, false);

    std::vector<std::size_t> seq = jacobsthal_sequence_up_to(m);
    for (std::size_t i = 0; i < seq.size(); ++i) {
        std::size_t j = seq[i];
        if (j >= 1 && j <= m) {
            std::size_t idx0 = j - 1;
            if (!used[idx0]) {
                order.push_back(idx0);
                used[idx0] = true;
            }
        }
    }
    for (std::size_t i = 0; i < m; ++i) {
        if (!used[i]) order.push_back(i);
    }
    return order;
}

// --- Ford-Johnson merge-insertion (template, C++98-friendly) ---
// Input: any Container with random-access iterators, operator[], push_back, insert, begin/end.
// Returns a sorted container (copy) using the big/small pair separation and Jacobsthal insertion order.
// Non-template implementation for std::vector<int>
std::vector<int> ford_johnson_sort(const std::vector<int> &src)
{
    if (src.size() <= 1) return src;

    std::vector<int> big;
    std::vector<int> small;
    std::vector<int>::const_iterator it = src.begin();

    while (it != src.end()) {
        int first = *it;
        ++it;
        if (it == src.end()) {
            big.push_back(first);
            break;
        }
        int second = *it;
        ++it;
        if (first > second) {
            big.push_back(first);
            small.push_back(second);
        } else {
            big.push_back(second);
            small.push_back(first);
        }
    }

    big = ford_johnson_sort(big);

    std::vector<std::size_t> order = jacobsthal_order(small.size());
    for (std::size_t i = 0; i < order.size(); ++i) {
        std::size_t idx = order[i];
        int val = small[idx];
        std::vector<int>::iterator pos = std::lower_bound(big.begin(), big.end(), val);
        big.insert(pos, val);
    }

    return big;
}

// Non-template implementation for std::deque<int>
std::deque<int> ford_johnson_sort(const std::deque<int> &src)
{
    if (src.size() <= 1) return src;

    std::deque<int> big;
    std::deque<int> small;
    std::deque<int>::const_iterator it = src.begin();

    while (it != src.end()) {
        int first = *it;
        ++it;
        if (it == src.end()) {
            big.push_back(first);
            break;
        }
        int second = *it;
        ++it;
        if (first > second) {
            big.push_back(first);
            small.push_back(second);
        } else {
            big.push_back(second);
            small.push_back(first);
        }
    }

    big = ford_johnson_sort(big);

    std::vector<std::size_t> order = jacobsthal_order(small.size());
    for (std::size_t i = 0; i < order.size(); ++i) {
        std::size_t idx = order[i];
        int val = small[idx];
        std::deque<int>::iterator pos = std::lower_bound(big.begin(), big.end(), val);
        big.insert(pos, val);
    }

    return big;
}