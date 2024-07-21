#pragma once

struct FormatParams
{
	FormatParams() = default;

	FormatParams(uint32_t text_alignment, uint32_t paragraph_alignment, uint32_t word_wrapping, uint32_t trimming_granularity)
		: m_text_alignment(static_cast<DWRITE_TEXT_ALIGNMENT>(text_alignment))
		, m_paragraph_alignment(static_cast<DWRITE_PARAGRAPH_ALIGNMENT>(paragraph_alignment))
		, m_word_wrapping(static_cast<DWRITE_WORD_WRAPPING>(word_wrapping))
	{
		m_trimming.granularity = static_cast<DWRITE_TRIMMING_GRANULARITY>(trimming_granularity);
	}

	DWRITE_TEXT_ALIGNMENT m_text_alignment{};
	DWRITE_PARAGRAPH_ALIGNMENT m_paragraph_alignment{};
	DWRITE_WORD_WRAPPING m_word_wrapping{};
	DWRITE_TRIMMING m_trimming{};
};
