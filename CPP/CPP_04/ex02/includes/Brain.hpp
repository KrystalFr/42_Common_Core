/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Brain.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 04:29:24 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 04:37:54 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BRAIN_HPP
# define BRAIN_HPP

# include <string>
# include <iostream>

class Brain
{
	private:
		std::string ideas[100];
		
	public:
    	Brain(void);
    	Brain(const Brain& other);
   		Brain& operator=(const Brain& other);
    	~Brain(void);

		std::string getIdea(int id) const;
    	void setIdea(int id, const std::string& idea);
};

#endif
