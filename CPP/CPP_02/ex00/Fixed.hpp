/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fixed.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 23:28:55 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/18 23:41:27 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FIXED_HPP
# define FIXED_HPP

# include <iostream>

class Fixed
{
	private:
		int					rawBits;
		static const int	fractBits;
	public:
    	Fixed(void);
    	Fixed(const Fixed& other);
   		Fixed& operator=(const Fixed& other);
    	~Fixed(void);

		int getRawBits(void) const;
		void setRawBits(int const raw);
};

#endif
