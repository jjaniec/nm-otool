/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_header_info.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/06 14:27:45 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/13 19:06:04 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static uint32_t		get_ncmds(t_ft_nm_file *file, t_ft_nm_hdrinfo *hdrinfo)
{
	struct mach_header		hdr;
	struct mach_header_64	hdr64;

	slseek(file, hdrinfo->fat_offset, SLSEEK_SET);
	if (hdrinfo->is_64)
	{
		sseek_read(file, &hdr64, hdrinfo->machhdr_size);
		if (!hdrinfo->is_be)
			swap_byte_range(&hdr64, hdrinfo->machhdr_size);
		// printf("Mach header 64:\nmagic\t\tcputype\t\tcpusubtype\tfiletype\tncmds\t\tsizeofcmds\tflags\n%x\t%u\t%u\t%u\t\t%u\t\t%u\t\t%x\n", hdr64.magic, hdr64.cputype, hdr64.cpusubtype, hdr64.filetype, hdr64.ncmds, hdr64.sizeofcmds, hdr64.flags);
		return (hdr64.ncmds);
	}
	else
	{
		sseek_read(file, &hdr, hdrinfo->machhdr_size);
		if (!hdrinfo->is_be)
			swap_byte_range(&hdr, hdrinfo->machhdr_size);
		return (hdr.ncmds);
	}
	return (0);
}

static int			init_fat_arch_values(t_ft_nm_hdrinfo *hdrinfo, struct fat_arch *arch, uint32_t fat_magic)
{
	if (arch)
	{
		hdrinfo->fat_offset = is_big_endian(fat_magic) ? (fat_magic) : swap_32bit(arch->offset);
		hdrinfo->fat_align = is_big_endian(fat_magic) ? (fat_magic) : swap_32bit(arch->align);
		hdrinfo->fat_size = is_big_endian(fat_magic) ? (fat_magic) : swap_32bit(arch->size);
	}
	else
	{
		hdrinfo->fat_offset = 0;
		hdrinfo->fat_align = 0;
		hdrinfo->fat_size = 0;
	}
	return (0);
}

static t_ft_nm_hdrinfo	*init_macho_header(t_ft_nm_file *file, \
							t_ft_nm_hdrinfo *hdrinfo, struct fat_arch *arch, uint32_t start_magic)
{
	uint32_t			magic;

	init_fat_arch_values(hdrinfo, arch, start_magic);
	if (arch)
	{
		slseek(file, hdrinfo->fat_offset, SLSEEK_SET);
		sseek_read(file, &magic, sizeof(uint32_t));
	}
	else
		magic = start_magic;
	slseek(file, hdrinfo->fat_offset + sizeof(uint32_t), SLSEEK_SET);
	sseek_read(file, &(hdrinfo->cpu_type), sizeof(cpu_type_t));
	if (hdrinfo->is_be)
		swap_byte_range(&(hdrinfo->cpu_type), sizeof(cpu_type_t));
	hdrinfo->file = file;
	hdrinfo->is_64 = is_magic_64(magic);
	hdrinfo->is_be = is_big_endian(magic);
	hdrinfo->magic = magic;
	hdrinfo->text_nsect = 1;
	hdrinfo->data_nsect = 2;
	hdrinfo->bss_nsect = NO_SECT;
	hdrinfo->machhdr_size = (hdrinfo->is_64) ? \
		(sizeof(struct mach_header_64)) : (sizeof(struct mach_header));
	if ((hdrinfo->ncmds = get_ncmds(file, hdrinfo)) == 0)
		return (NULL);
	dprintf(2, "Init new header: magic: %x, is_be: %d, is_64: %d, cpu_type: %u, offset %u, size: %zu, fat_size: %u, ncdms: %u\n", \
		hdrinfo->magic, hdrinfo->is_be, hdrinfo->is_64, hdrinfo->cpu_type, hdrinfo->fat_offset, hdrinfo->machhdr_size, hdrinfo->fat_size, hdrinfo->ncmds);
	return (hdrinfo);
}

static int			handle_fat_header(t_ft_nm_file *file, t_ft_nm_hdrinfo *hdrinfo, uint32_t nfat_arch, uint32_t magic)
{
	struct fat_arch		arch;
	uint32_t			fat_header_idx;
	t_ft_nm_hdrinfo		*cur_mach_header;
	t_ft_nm_hdrinfo		*new_mach_header;

	fat_header_idx = 0;
	cur_mach_header = NULL;
	while (fat_header_idx < nfat_arch)
	{
		slseek(file, sizeof(struct fat_header) + (sizeof(struct fat_arch) * fat_header_idx), SLSEEK_SET);
		sseek_read(file, &arch, sizeof(struct fat_arch));
		if (cur_mach_header)
		{
			new_mach_header = malloc(sizeof(t_ft_nm_hdrinfo));
			if (!init_macho_header(file, new_mach_header, &arch, magic))
				return (1);
			if (check_hdr_overlap(cur_mach_header, (uint32_t)file->seek_ptr))
			{
				dprintf(2, "Overlap of previous header\n");
				return (1);
			}
			cur_mach_header->next = new_mach_header;
			cur_mach_header = new_mach_header;
		}
		else
		{
			if (!(cur_mach_header = init_macho_header(file, hdrinfo, &arch, magic)))
				return (1);
		}
		fat_header_idx++;
		dprintf(2, "fat_header_idx: %d / %" PRIu32 " - cur_mach_header: %p, offset: %x\n", fat_header_idx, nfat_arch, cur_mach_header, cur_mach_header->fat_offset);
	}
	return (0);
}

int					init_header_info(t_ft_nm_file *file, \
						t_ft_nm_hdrinfo *hdrinfo)
{
	uint32_t			magic;
	uint32_t			nfat_arch;

	(void)file;
	sseek_read(file, &magic, sizeof(uint32_t));
	if (is_magic_fat(magic))
	{
		sseek_read(file, &nfat_arch, sizeof(uint32_t));
		if (!is_big_endian(magic))
			nfat_arch = swap_32bit(nfat_arch);
		return (handle_fat_header(file, hdrinfo, nfat_arch, magic));
	}
	else if (is_magic_mach(magic))
		return (init_macho_header(file, hdrinfo, NULL, magic) == NULL);
	return (1);
}
