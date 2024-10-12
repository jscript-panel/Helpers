#include "stdafx.hpp"
#include "KMeans.hpp"

namespace KMeans
{
	Cluster::Cluster(const KPoint& point) : m_central_values(point.m_values)
	{
		m_points.emplace_back(point);
	}

	double Cluster::calc_dist(const KPoint& point) const
	{
		double dist{};

		for (auto&& [central_value, value, multiplier] : std::views::zip(m_central_values, point.m_values, kMultipliers))
		{
			dist += square_then_multiply(central_value - value, multiplier);
		}

		return dist;
	}

	double Cluster::square_then_multiply(double num, double by) const
	{
		return num * num * by;
	}

	int Cluster::get_colour()
	{
		const auto c = RGB(get_colour_component(0), get_colour_component(1), get_colour_component(2));
		return js::to_argb(c);
	}

	size_t Cluster::get_total_points() const
	{
		size_t total{};

		for (auto&& point : m_points)
		{
			total += point.m_pixel_count;
		}

		return total;
	}

	uint8_t Cluster::get_colour_component(size_t index)
	{
		return static_cast<uint8_t>(m_central_values[index]);
	}

	void Cluster::remove_point(size_t point_id)
	{
		std::erase_if(m_points, [point_id](const KPoint& point)
			{
				return point.m_point_id == point_id;
			});
	}

	KMeans::KMeans(const KPoints& points, size_t count) : m_points(points), m_count(count) {}

	Clusters KMeans::run()
	{
		const size_t max_count = std::max(m_count, kMaxCount);
		const size_t count = std::min(max_count, m_points.size());

		for (const size_t i : std::views::iota(size_t{}, count))
		{
			const size_t index_point = i * m_points.size() / count;
			m_points[index_point].m_cluster_id = i;
			Cluster cluster(m_points[index_point]);
			m_clusters.emplace_back(cluster);
		}

		for ([[maybe_unused]] const size_t iteration : std::views::iota(size_t{}, kMaxIterations))
		{
			bool done = true;

			for (auto&& point : m_points)
			{
				const size_t old_cluster_id = point.m_cluster_id;
				const size_t nearest_centre_id = get_nearest_centre_id(point);

				if (old_cluster_id != nearest_centre_id)
				{
					if (old_cluster_id != SIZE_MAX)
					{
						m_clusters[old_cluster_id].remove_point(point.m_point_id);
					}

					point.m_cluster_id = nearest_centre_id;
					m_clusters[nearest_centre_id].m_points.emplace_back(point);
					done = false;
				}
			}

			for (auto&& cluster : m_clusters)
			{
				const size_t cluster_total_points = cluster.get_total_points();

				if (cluster_total_points == 0)
					continue;

				for (auto&& [index, value] : std::views::enumerate(cluster.m_central_values))
				{
					double sum{};

					for (auto&& point : cluster.m_points)
					{
						sum += point.m_values[index] * point.m_pixel_count;
					}

					value = sum / cluster_total_points;
				}
			}

			if (done)
				break;
		}

		std::ranges::sort(m_clusters, [](const Cluster& a, const Cluster& b)
			{
				return a.get_total_points() > b.get_total_points();
			});

		return m_clusters | std::views::take(m_count) | std::ranges::to<Clusters>();
	}

	size_t KMeans::get_nearest_centre_id(const KPoint& point)
	{
		auto transform = [point](const Cluster& cluster) { return cluster.calc_dist(point); };
		auto view = m_clusters | std::views::transform(transform);
		const auto it = std::ranges::min_element(view);
		return std::ranges::distance(view.begin(), it);
	}

	Colours GetColours(IWICBitmap* bitmap, size_t count)
	{
		ColourCounters colour_counters;
		KPoints points;

		if FAILED(GetColourCounters(bitmap, colour_counters))
			return Colours();

		for (auto&& [index, value] : std::views::enumerate(colour_counters))
		{
			points.emplace_back(index, value.first, value.second);
		}

		auto kmeans = KMeans(points, count);
		auto clusters = kmeans.run();
		auto transform = [](auto&& cluster) { return cluster.get_colour(); };
		return clusters | std::views::transform(transform) | std::ranges::to<Colours>();
	}

	HRESULT GetColourCounters(IWICBitmap* bitmap, ColourCounters& colour_counters)
	{
		static constexpr int colours_length = 200 * 200;
		static const auto size = D2D1::SizeU(200U, 200U);
		static const auto rect = js::to_WICRect(size);

		uint32_t data_size{};
		uint8_t* data{};
		wil::com_ptr_t<IWICBitmapScaler> scaler;
		wil::com_ptr_t<IWICBitmap> copy;
		wil::com_ptr_t<IWICBitmapLock> lock;

		RETURN_IF_FAILED(factory::imaging->CreateBitmapScaler(&scaler));
		RETURN_IF_FAILED(scaler->Initialize(bitmap, size.width, size.height, WICBitmapInterpolationModeFant));
		RETURN_IF_FAILED(factory::imaging->CreateBitmapFromSource(scaler.get(), WICBitmapCacheOnDemand, &copy));
		RETURN_IF_FAILED(copy->Lock(&rect, WICBitmapLockWrite, &lock));
		RETURN_IF_FAILED(lock->GetDataPointer(&data_size, &data));
		RETURN_HR_IF_NULL(E_POINTER, data);

		auto colours = reinterpret_cast<const uint32_t*>(data);

		for (const int i : std::views::iota(0, colours_length))
		{
			ColourValues values{};

			std::ranges::transform(kShifts, values.begin(), [colour = colours[i]](const int shift)
				{
					const uint8_t value = (colour >> shift) & UINT8_MAX;
					return static_cast<double>(value > 251 ? UINT8_MAX : (value + 4) & 0xf8);
				});

			++colour_counters[values];
		}

		return S_OK;
	}
}
