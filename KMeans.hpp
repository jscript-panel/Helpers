#pragma once

namespace KMeans
{
	static constexpr size_t kMaxCount = 14U;
	static constexpr size_t kMaxIterations = 12U;
	static constexpr std::array kColourIndexes = { 0U, 1U, 2U };
	static constexpr std::array kMultipliers = { 2.0, 4.0, 3.0 };
	static constexpr std::array kShifts = { RED_SHIFT, GREEN_SHIFT, BLUE_SHIFT };
	using Colours = std::vector<int>;
	using ColourValues = std::array<double, kColourIndexes.size()>;
	using ColourCounters = std::map<ColourValues, size_t>;

	struct KPoint
	{
		KPoint(size_t point_id, const ColourValues& values, size_t pixel_count) : m_point_id(point_id), m_values(values), m_pixel_count(pixel_count), m_cluster_id(SIZE_MAX) {}

		ColourValues m_values{};
		size_t m_cluster_id{}, m_pixel_count{}, m_point_id{};
	};

	using KPoints = std::vector<KPoint>;

	class Cluster
	{
	public:
		Cluster(const KPoint& point);

		double calc_dist(const KPoint& point) const;
		double square_then_multiply(double num, double by) const;
		int get_colour();
		size_t get_total_points() const;
		uint8_t get_colour_component(size_t index);
		void remove_point(size_t point_id);

		ColourValues m_central_values{};
		KPoints m_points;
	};

	using Clusters = std::vector<Cluster>;

	class KMeans
	{
	public:
		KMeans(const KPoints& points, size_t count);

		Clusters run();

	private:
		size_t get_nearest_centre_id(const KPoint& point);

		Clusters m_clusters;
		KPoints m_points;
		size_t m_count{};
	};

	Colours GetColours(IWICBitmap* bitmap, size_t count);
	HRESULT GetColourCounters(IWICBitmap* bitmap, ColourCounters& colour_counters);
}
