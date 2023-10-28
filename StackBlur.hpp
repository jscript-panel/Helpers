#pragma once

struct StackRGBA;

class StackBlur
{
public:
	StackBlur(uint8_t p_radius, uint32_t p_width, uint32_t p_height);

	void Run(uint8_t* src);

private:
	void InitPtr(uint8_t* ptr, const StackRGBA& sum);
	void StackBlurThread(uint32_t step, uint32_t core, uint8_t* src, uint8_t* stack);

	uint8_t radius{}, shr_sum{};
	uint16_t mul_sum{};
	uint32_t div{}, cores{}, width{}, height{};
};
