/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soutin <soutin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/13 18:02:53 by soutin            #+#    #+#             */
/*   Updated: 2024/08/21 21:27:44 by soutin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

bool verify_string_format(const std::string &input_string)
{
	for (size_t i = 0; i < input_string.size(); i++)
	{
		char c = input_string[i];
		if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || std::string("-[]\\`^{}").find(c) != std::string::npos))
			return (false);
	}
	return (true);
}

