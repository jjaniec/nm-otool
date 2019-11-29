/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 16:09:55 by jjaniec           #+#    #+#             */
/*   Updated: 2019/11/29 19:52:17 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static int			sseek_read(t_ft_nm_file *file, void *buf, size_t size)
{
	size_t	i;

	i = 0;
	while (i < (*file).totsiz - (size_t)((*file).seek_ptr - (*file).content))
	{
		*((unsigned char *)buf + i) = *((unsigned char *)(*file).seek_ptr);
		(*file).seek_ptr += sizeof(unsigned char);
		if (++i == size)
			break ;
	}
	return (i);
}

static off_t		slseek(t_ft_nm_file *file, off_t offset, int whence)
{
	off_t	r;

	r = -1;
	if (whence == SLSEEK_CUR)
	{
		if ((offset <= 0 && (size_t)offset < (size_t)((*file).seek_ptr - (*file).content)) || \
			((size_t)offset + (size_t)((*file).seek_ptr - (*file).content) <= (*file).totsiz))
		{
			(*file).seek_ptr += offset;
			r = offset;
		}
	}
	else if (whence == SLSEEK_SET)
	{
		if (offset >= 0 && (*file).totsiz >= (size_t)offset)
		{
			(*file).seek_ptr = (*file).content + offset;
			r = offset;
		}
	}
	printf("File totsize: %zu - content %p: seek_ptr %p\n", (*file).totsiz, (*file).content, (*file).seek_ptr);
	return (r);
}

static uint32_t	get_ncmds(t_ft_nm_file *file, t_nm_info *fileinfo)
{
	struct mach_header		hdr;
	struct mach_header_64	hdr64;

	slseek(file, 0, SLSEEK_SET);
	if (fileinfo->is_64)
	{
		sseek_read(file, &hdr64, fileinfo->machhdr_size);
		if (!fileinfo->is_be)
			swap_byte_range(&hdr64, fileinfo->machhdr_size);
		return (hdr64.ncmds);
	}
	else if (!fileinfo->is_64)
	{
		sseek_read(file, &hdr, fileinfo->machhdr_size);
		if (!fileinfo->is_be)
			swap_byte_range(&hdr, fileinfo->machhdr_size);
		return (hdr.ncmds);
	}
	return (0);
}

static int		init_file_info(t_ft_nm_file *file, t_nm_info *fileinfo)
{
	uint32_t			magic;

	sseek_read(file, &magic, sizeof(uint32_t));
	fileinfo->is_64 = is_magic_64(magic);
	fileinfo->is_be = is_big_endian(magic);
	fileinfo->magic = magic;
	fileinfo->machhdr_size = (fileinfo->is_64) ? \
		(sizeof(struct mach_header_64)) : (sizeof(struct mach_header));
	// slseek(file, 0, SLSEEK_SET);
	if ((fileinfo->ncmds = get_ncmds(file, fileinfo)) == 0)
		return (-1);
	return (0);
}

int				ft_nm(t_ft_nm_file *file)
{
	t_nm_info		fileinfo;

	init_file_info(file, &fileinfo);
	printf("file: %50s\nMagic: %x - is_64: %d - is_be: %d - ncmds: %u - machhdr_size: %zu\n", file->content, fileinfo.magic, fileinfo.is_64, fileinfo.is_be, fileinfo.ncmds, fileinfo.machhdr_size);
	// printf("%u", fileinfo.ncmds);
	return (0);
}
