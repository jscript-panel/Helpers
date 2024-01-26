#pragma once

struct StackRGBA;

class StackBlur
{
public:
	StackBlur(uint8_t p_radius, const D2D1_SIZE_U& p_size);

	void Run(uint8_t* src);

private:
	void InitPtr(uint8_t* ptr, const StackRGBA& sum);
	void StackBlurThread(uint32_t step, uint32_t core, uint8_t* src, uint8_t* stack);

	D2D1_SIZE_U size{};
	uint8_t radius{}, shr_sum{};
	uint16_t mul_sum{};
	uint32_t div{}, cores{};
};
