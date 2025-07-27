// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2024 Daniil Goncharov <neargye@gmail.com>.
// Copyright (c) 2022 - 2023 Bela Schaum <schaumb@gmail.com>.
//
// Permission is hereby  granted, free of charge, to any  person obtaining a copy
// of this software and associated  documentation files (the "Software"), to deal
// in the Software  without restriction, including without  limitation the rights
// to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
// copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#if defined(__clang__)
#  pragma clang diagnostic push
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4244) // warning C4244: 'argument': conversion from 'const T' to 'unsigned int', possible loss of data.
#endif

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <magic_enum/magic_enum_containers.hpp>
#include <magic_enum/magic_enum_iostream.hpp>

#include <functional>
#include <algorithm>
#include <iostream>

enum class TColor { RED = 1, GREEN = 2, BLUE = 4 };
template <>
struct magic_enum::customize::enum_range<TColor> {
  static constexpr bool is_flags = true;
};

enum class Empty {};

struct RGB {

  std::uint8_t r {};
  std::uint8_t g {};
  std::uint8_t b {};

  [[nodiscard]] constexpr bool empty() { return std::equal_to{}(r, g) && std::equal_to{}(g, b) && std::equal_to{}(b, 0); }

  [[nodiscard]] constexpr bool operator==(RGB rgb) const noexcept { return std::equal_to{}(r, rgb.r) && std::equal_to{}(g, rgb.g) && std::equal_to{}(b, rgb.b); }

  friend std::ostream& operator<<(std::ostream& ostream, RGB rgb) {

    ostream << "R=" << static_cast<std::uint32_t>(rgb.r) << " G=" << static_cast<std::uint32_t>(rgb.g) << " B=" << static_cast<std::uint32_t>(rgb.b);
    return ostream;
  }
};

template <typename T> bool check_const([[maybe_unused]]T& element) { return false; }
template <typename T> bool check_const([[maybe_unused]]T const& element) { return true; }

constexpr std::uint8_t color_max = std::numeric_limits<std::uint8_t>::max();

TEST_CASE("containers_array") {

  using namespace magic_enum::bitwise_operators;
  using namespace magic_enum::ostream_operators;

  constexpr magic_enum::containers::array<TColor, RGB> color_rgb_initializer {{{{color_max, 0, 0}, {0, color_max, 0}, {0, 0, color_max}}}};
  REQUIRE(color_rgb_initializer.at(TColor::RED) == RGB{color_max, 0, 0});
  REQUIRE(color_rgb_initializer.at(TColor::GREEN) == RGB{0, color_max, 0});
  REQUIRE(color_rgb_initializer.at(TColor::BLUE) == RGB{0, 0, color_max});

  /* BUG: a sort will not survive the data integration */
  magic_enum::containers::array<TColor, std::uint8_t> color_rgb_container_int{{1U, 4U, 2U}};

  // ENC: Direct convert to std::array
  // std::array compare_before {1U, 4U, 2U};
  constexpr magic_enum::containers::array<TColor, std::uint8_t> compare_before{{1U, 4U, 2U}};
  REQUIRE(color_rgb_container_int == compare_before);

  constexpr auto colors = magic_enum::enum_values<TColor>();

  std::ignore = magic_enum::containers::get<0>(compare_before);
  std::ignore = magic_enum::containers::get<1>(compare_before);
  std::ignore = magic_enum::containers::get<2>(compare_before);

  std::ignore = magic_enum::containers::get<TColor::RED>(compare_before);
  std::ignore = magic_enum::containers::get<TColor::GREEN>(compare_before);
  std::ignore = magic_enum::containers::get<TColor::BLUE>(compare_before);

  REQUIRE(std::make_pair(colors[0], color_rgb_container_int[colors[0]]) == std::make_pair<TColor, std::uint8_t>(TColor::RED, 1U));
  REQUIRE(std::make_pair(colors[1], color_rgb_container_int[colors[1]]) == std::make_pair<TColor, std::uint8_t>(TColor::GREEN, 4U));
  REQUIRE(std::make_pair(colors[2], color_rgb_container_int[colors[2]]) == std::make_pair<TColor, std::uint8_t>(TColor::BLUE, 2U));

  std::sort(std::begin(color_rgb_container_int), std::end(color_rgb_container_int));

  // Missing: Direct convert to std::array
  // std::array compare_after {1U, 2U, 4U};
  constexpr magic_enum::containers::array<TColor, std::uint8_t> compare_after{{1U, 2U, 4U}};
  REQUIRE(color_rgb_container_int == compare_after);

  std::ignore = magic_enum::containers::get<0>(compare_after);
  std::ignore = magic_enum::containers::get<1>(compare_after);
  std::ignore = magic_enum::containers::get<2>(compare_after);

  std::ignore = magic_enum::containers::get<TColor::RED>(compare_after);
  std::ignore = magic_enum::containers::get<TColor::GREEN>(compare_after);
  std::ignore = magic_enum::containers::get<TColor::BLUE>(compare_after);

  REQUIRE(std::make_pair(colors[0], color_rgb_container_int[colors[0]]) == std::make_pair<TColor, std::uint8_t>(TColor::RED, 1U));
  REQUIRE(std::make_pair(colors[1], color_rgb_container_int[colors[1]]) == std::make_pair<TColor, std::uint8_t>(TColor::GREEN, 2U));
  REQUIRE(std::make_pair(colors[2], color_rgb_container_int[colors[2]]) == std::make_pair<TColor, std::uint8_t>(TColor::BLUE, 4U));

  auto color_rgb_container = magic_enum::containers::array<TColor, RGB>();
  REQUIRE_FALSE(color_rgb_container.empty());
  REQUIRE(color_rgb_container.size() == 3);
  REQUIRE(magic_enum::enum_count<TColor>() == color_rgb_container.size());

  REQUIRE(color_rgb_container.at(TColor::RED).empty());
  REQUIRE(color_rgb_container.at(TColor::GREEN).empty());
  REQUIRE(color_rgb_container.at(TColor::BLUE).empty());
  REQUIRE_THROWS(color_rgb_container.at(TColor::BLUE|TColor::GREEN).empty());

  color_rgb_container[TColor::RED] = {color_max, 0, 0};
  color_rgb_container[TColor::GREEN] = {0, color_max, 0};
  color_rgb_container[TColor::BLUE] = {0, 0, color_max};

  REQUIRE(color_rgb_container.at(TColor::RED) == RGB{color_max, 0, 0});
  REQUIRE(color_rgb_container.at(TColor::GREEN) == RGB{0, color_max, 0});
  REQUIRE(color_rgb_container.at(TColor::BLUE) == RGB{0, 0, color_max});

  REQUIRE(color_rgb_container.front() == RGB{color_max, 0, 0});
  REQUIRE(color_rgb_container.back() == RGB{0, 0, color_max});

  REQUIRE(magic_enum::containers::get<TColor::RED>(color_rgb_container) == RGB{color_max, 0, 0});
  REQUIRE(magic_enum::containers::get<TColor::GREEN>(color_rgb_container) == RGB{0, color_max, 0});
  REQUIRE(magic_enum::containers::get<TColor::BLUE>(color_rgb_container) == RGB{0, 0, color_max});

  auto iterator = color_rgb_container.begin();
  REQUIRE_FALSE(check_const(iterator));
  REQUIRE(check_const(color_rgb_container.begin()));
  REQUIRE(check_const(color_rgb_container.cbegin()));

  auto color_rgb_container_compare = magic_enum::containers::array<TColor, RGB>();
  color_rgb_container_compare.fill({color_max, color_max, color_max});
  REQUIRE_FALSE(color_rgb_container == color_rgb_container_compare);

  color_rgb_container_compare[TColor::RED] = {color_max, 0, 0};
  color_rgb_container_compare[TColor::GREEN] = {0, color_max, 0};
  color_rgb_container_compare[TColor::BLUE] = {0, 0, color_max};
  REQUIRE(color_rgb_container == color_rgb_container_compare);

  constexpr auto from_to_array = magic_enum::containers::to_array<TColor, RGB>({{color_max, 0, 0}, {0, color_max, 0}, {0, 0, color_max}});
  REQUIRE(from_to_array.at(TColor::RED) == RGB{color_max, 0, 0});
  REQUIRE(from_to_array.at(TColor::GREEN) == RGB{0, color_max, 0});
  REQUIRE(from_to_array.at(TColor::BLUE) == RGB{0, 0, color_max});
}

TEST_CASE("containers_bitset") {

  using namespace magic_enum::bitwise_operators;

  auto color_bitset = magic_enum::containers::bitset<TColor>();
  REQUIRE(color_bitset.to_string().empty());
  REQUIRE(color_bitset.size() == 3);
  REQUIRE(magic_enum::enum_count<TColor>() == color_bitset.size());
  REQUIRE_FALSE(color_bitset.all());
  REQUIRE_FALSE(color_bitset.any());
  REQUIRE(color_bitset.none());
  REQUIRE(color_bitset.count() == 0);

  color_bitset.set(TColor::GREEN);
  REQUIRE_FALSE(color_bitset.all());
  REQUIRE(color_bitset.any());
  REQUIRE_FALSE(color_bitset.none());
  REQUIRE(color_bitset.count() == 1);
  REQUIRE_FALSE(color_bitset.test(TColor::RED));
  REQUIRE(color_bitset.test(TColor::GREEN));
  REQUIRE_FALSE(color_bitset.test(TColor::BLUE));

  color_bitset.set(TColor::BLUE);
  REQUIRE_FALSE(color_bitset.all());
  REQUIRE(color_bitset.any());
  REQUIRE_FALSE(color_bitset.none());
  REQUIRE(color_bitset.count() == 2);
  REQUIRE_FALSE(color_bitset.test(TColor::RED));
  REQUIRE(color_bitset.test(TColor::GREEN));
  REQUIRE(color_bitset.test(TColor::BLUE));

  color_bitset.set(TColor::RED);
  REQUIRE(color_bitset.all());
  REQUIRE(color_bitset.any());
  REQUIRE_FALSE(color_bitset.none());
  REQUIRE(color_bitset.count() == 3);
  REQUIRE(color_bitset.test(TColor::RED));
  REQUIRE(color_bitset.test(TColor::GREEN));
  REQUIRE(color_bitset.test(TColor::BLUE));

  color_bitset.reset();
  REQUIRE_FALSE(color_bitset.all());
  REQUIRE_FALSE(color_bitset.any());
  REQUIRE(color_bitset.none());
  REQUIRE(color_bitset.count() == 0);
  REQUIRE_FALSE(color_bitset.test(TColor::RED));
  REQUIRE_FALSE(color_bitset.test(TColor::GREEN));
  REQUIRE_FALSE(color_bitset.test(TColor::BLUE));

  color_bitset.set(TColor::RED);
  REQUIRE(color_bitset.test(TColor::RED));
  REQUIRE_FALSE(color_bitset.test(TColor::GREEN));
  REQUIRE_FALSE(color_bitset.test(TColor::BLUE));

  color_bitset.flip();
  REQUIRE_FALSE(color_bitset.test(TColor::RED));
  REQUIRE(color_bitset.test(TColor::GREEN));
  REQUIRE(color_bitset.test(TColor::BLUE));

  constexpr magic_enum::containers::bitset<TColor> color_bitset_all {TColor::RED|TColor::GREEN|TColor::BLUE};
  REQUIRE(color_bitset_all.to_string() == "RED|GREEN|BLUE");
  REQUIRE(color_bitset_all.to_string( {}, '0', '1' ) == "111");
  REQUIRE(color_bitset_all.to_ulong( {} ) == 7);
  REQUIRE(color_bitset_all.to_ullong( {} ) == 7);
  REQUIRE(color_bitset_all.all());
  REQUIRE(color_bitset_all.any());
  REQUIRE_FALSE(color_bitset_all.none());

  constexpr magic_enum::containers::bitset<TColor> color_bitset_red_green {TColor::RED|TColor::GREEN};
  REQUIRE(color_bitset_red_green.to_string() == "RED|GREEN");
  REQUIRE(color_bitset_red_green.to_string( {}, '0', '1' ) == "110");
  REQUIRE(color_bitset_red_green.to_ulong( {} ) == 3);
  REQUIRE(color_bitset_red_green.to_ullong( {} ) == 3);
  REQUIRE_FALSE(color_bitset_red_green.all());
  REQUIRE(color_bitset_red_green.any());
  REQUIRE_FALSE(color_bitset_red_green.none());
}

TEST_CASE("containers_set") {

  using namespace magic_enum::bitwise_operators;
  using namespace magic_enum::ostream_operators;

  auto color_set = magic_enum::containers::set<TColor>();
  REQUIRE(color_set.empty());
  REQUIRE(color_set.size() == 0);
  REQUIRE_FALSE(magic_enum::enum_count<TColor>() == color_set.size());

  color_set.insert(TColor::RED);
  std::ignore = color_set.insert(TColor::RED);
  color_set.insert(TColor::GREEN);
  color_set.insert(TColor::BLUE);
  color_set.insert(TColor::RED);
  color_set.insert(TColor::RED|TColor::GREEN);
  color_set.insert(TColor::RED|TColor::BLUE);
  color_set.insert(TColor::GREEN|TColor::BLUE);
  color_set.insert(TColor::RED|TColor::GREEN|TColor::BLUE);

  REQUIRE_FALSE(color_set.empty());
  REQUIRE(color_set.size() == 3);
  REQUIRE(magic_enum::enum_count<TColor>() == color_set.size());
  color_set.erase(TColor::RED);
  color_set.erase(TColor::GREEN);
  REQUIRE(magic_enum::enum_count<TColor>() - 2 == color_set.size());
  REQUIRE(color_set.count(TColor::RED) == 0);
  REQUIRE_FALSE(color_set.contains(TColor::GREEN));
  REQUIRE(color_set.contains(TColor::BLUE));

  auto color_set_compare = magic_enum::containers::set<TColor>();
  color_set_compare.insert(TColor::BLUE);
  color_set_compare.insert(TColor::RED);
  color_set_compare.insert(TColor::GREEN);

  constexpr magic_enum::containers::set color_set_filled = {TColor::RED, TColor::GREEN, TColor::BLUE};
  REQUIRE_FALSE(color_set_filled.empty());
  REQUIRE(color_set_filled.size() == 3);
  REQUIRE(magic_enum::enum_count<TColor>() == color_set_filled.size());

  magic_enum::containers::set color_set_not_const = {TColor::RED, TColor::GREEN, TColor::BLUE};
  REQUIRE_FALSE(color_set_not_const.empty());
  REQUIRE(color_set_not_const.size() == 3);
  REQUIRE(magic_enum::enum_count<TColor>() == color_set_not_const.size());
  color_set_not_const.clear();
  REQUIRE(color_set_not_const.empty());
  REQUIRE(color_set_not_const.size() == 0);
  REQUIRE_FALSE(magic_enum::enum_count<TColor>() == color_set_not_const.size());
}

TEST_CASE("map_like_container") {

  using namespace magic_enum::ostream_operators;

  std::vector<std::pair<TColor, RGB>> map {{TColor::GREEN, {0, color_max, 0}}, {TColor::BLUE, {0, 0, color_max}}, {TColor::RED, {color_max, 0, 0}}};
  for (auto [key, value] : map) {

    std::cout << "Key=" << key << " Value=" << value << std::endl;
  }
  auto compare = [](std::pair<TColor, RGB>& lhs,
                    std::pair<TColor, RGB>& rhs) {
     return static_cast<std::int32_t>(lhs.first) < static_cast<std::int32_t>(rhs.first);
  };
  std::sort(std::begin(map), std::end(map), compare);
  for (auto [key, value] : map) {

    std::cout << "Key=" << key << " Value=" << value << std::endl;
  }
}

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif
