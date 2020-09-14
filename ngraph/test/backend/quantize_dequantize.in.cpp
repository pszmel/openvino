//*****************************************************************************
// Copyright 2017-2020 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//*****************************************************************************

#include "gtest/gtest.h"
#include "ngraph/ngraph.hpp"
#include "util/engine/test_engines.hpp"
#include "util/test_case.hpp"
#include "util/test_control.hpp"

NGRAPH_SUPPRESS_DEPRECATED_START

using namespace std;
using namespace ngraph;

static string s_manifest = "${MANIFEST}";
using TestEngine = test::ENGINE_CLASS_NAME(${BACKEND_NAME});

NGRAPH_TEST(${BACKEND_NAME}, quantize)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::f32;
    auto output_type = element::u8;

    typedef float input_c_type;
    typedef uint8_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_NEAREST_TOWARD_EVEN;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {2});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {1});
    auto quantize =
        make_shared<op::Quantize>(X, scale, offset, output_type, quantization_axes, round_mode);
    auto f = make_shared<Function>(quantize, ParameterVector{X});

    std::vector<input_c_type> x{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape, {1, 1, 2, 3, 3, 3, 4, 5, 5, 5, 6, 7});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, dequantize)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::u8;
    auto output_type = element::f32;

    typedef uint8_t input_c_type;
    typedef float output_c_type;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(output_type, scale_offset_shape, {2});
    auto offset = op::Constant::create(input_type, scale_offset_shape, {1});
    auto dequantize = make_shared<op::Dequantize>(X, scale, offset, output_type, quantization_axes);
    auto f = make_shared<Function>(dequantize, ParameterVector{X});

    std::vector<input_c_type> x{{1, 1, 2, 3, 3, 3, 4, 5, 5, 5, 6, 7}};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {0, 0, 2, 4, 4, 4, 6, 8, 8, 8, 10, 12});
    test_case.run(MIN_FLOAT_TOLERANCE_BITS);
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_zero_offset)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::f32;
    auto output_type = element::u8;

    typedef float input_c_type;
    typedef uint8_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_NEAREST_TOWARD_EVEN;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {2});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {0});
    auto quantize =
        make_shared<op::Quantize>(X, scale, offset, output_type, quantization_axes, round_mode);
    auto f = make_shared<Function>(quantize, ParameterVector{X});

    std::vector<input_c_type> x{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape, {0, 0, 1, 2, 2, 2, 3, 4, 4, 4, 5, 6});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, dequantize_zero_offset)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::u8;
    auto output_type = element::f32;

    typedef uint8_t input_c_type;
    typedef float output_c_type;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(output_type, scale_offset_shape, {2});
    auto offset = op::Constant::create(input_type, scale_offset_shape, {0});
    auto dequantize = make_shared<op::Dequantize>(X, scale, offset, output_type, quantization_axes);
    auto f = make_shared<Function>(dequantize, ParameterVector{X});

    std::vector<input_c_type> x{0, 0, 1, 2, 2, 2, 3, 4, 4, 4, 5, 6};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {0, 0, 2, 4, 4, 4, 6, 8, 8, 8, 10, 12});
    test_case.run(MIN_FLOAT_TOLERANCE_BITS);
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_axes)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape{4};
    AxisSet quantization_axes{0};

    auto input_type = element::f32;
    auto output_type = element::u8;

    typedef float input_c_type;
    typedef uint8_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_NEAREST_TOWARD_INFINITY;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {2, 3, 4, 5});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {10, 20, 30, 40});
    auto quantize =
        make_shared<op::Quantize>(X, scale, offset, output_type, quantization_axes, round_mode);
    auto f = make_shared<Function>(quantize, ParameterVector{X});
    std::vector<input_c_type> x{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {10, 11, 11, 21, 21, 22, 32, 32, 32, 42, 42, 42});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, dequantize_axes)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape{4};
    AxisSet quantization_axes{0};

    auto input_type = element::u8;
    auto output_type = element::f32;

    typedef uint8_t input_c_type;
    typedef float output_c_type;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(output_type, scale_offset_shape, {2, 3, 4, 5});
    auto offset = op::Constant::create(input_type, scale_offset_shape, {10, 20, 30, 40});
    auto dequantize = make_shared<op::Dequantize>(X, scale, offset, output_type, quantization_axes);
    auto f = make_shared<Function>(dequantize, ParameterVector{X});

    std::vector<input_c_type> x{10, 11, 11, 21, 21, 22, 32, 32, 32, 42, 42, 42};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {0, 2, 2, 3, 3, 6, 8, 8, 8, 10, 10, 10});
    test_case.run(MIN_FLOAT_TOLERANCE_BITS);
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_int8)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::f32;
    auto output_type = element::i8;

    typedef float input_c_type;
    typedef int8_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_NEAREST_TOWARD_EVEN;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {2});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {1});
    auto quantize =
        make_shared<op::Quantize>(X, scale, offset, output_type, quantization_axes, round_mode);
    auto f = make_shared<Function>(quantize, ParameterVector{X});

    std::vector<input_c_type> x{0, -1, 2, -3, 4, -5, 6, -7, 8, -9, 10, -11};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {1, 1, 2, -1, 3, -1, 4, -3, 5, -3, 6, -5});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, dequantize_int8)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::i8;
    auto output_type = element::f32;

    typedef int8_t input_c_type;
    typedef float output_c_type;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(output_type, scale_offset_shape, {2});
    auto offset = op::Constant::create(input_type, scale_offset_shape, {1});
    auto dequantize = make_shared<op::Dequantize>(X, scale, offset, output_type, quantization_axes);
    auto f = make_shared<Function>(dequantize, ParameterVector{X});

    std::vector<input_c_type> x{1, 1, 2, -1, 3, -1, 4, -3, 5, -3, 6, -5};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {0, 0, 2, -4, 4, -4, 6, -8, 8, -8, 10, -12});
    test_case.run(MIN_FLOAT_TOLERANCE_BITS);
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_int8_zero_offset)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::f32;
    auto output_type = element::i8;

    typedef float input_c_type;
    typedef int8_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_NEAREST_TOWARD_EVEN;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {2});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {0});
    auto quantize =
        make_shared<op::Quantize>(X, scale, offset, output_type, quantization_axes, round_mode);
    auto f = make_shared<Function>(quantize, ParameterVector{X});

    std::vector<input_c_type> x{0, -1, 2, -3, 4, -5, 6, -7, 8, -9, 10, -11};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {0, 0, 1, -2, 2, -2, 3, -4, 4, -4, 5, -6});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, dequantize_int8_zero_offset)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::i8;
    auto output_type = element::f32;

    typedef int8_t input_c_type;
    typedef float output_c_type;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(output_type, scale_offset_shape, {2});
    auto offset = op::Constant::create(input_type, scale_offset_shape, {0});
    auto dequantize = make_shared<op::Dequantize>(X, scale, offset, output_type, quantization_axes);
    auto f = make_shared<Function>(dequantize, ParameterVector{X});

    std::vector<input_c_type> x{0, 0, 1, -2, 2, -2, 3, -4, 4, -4, 5, -6};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {0, 0, 2, -4, 4, -4, 6, -8, 8, -8, 10, -12});
    test_case.run(MIN_FLOAT_TOLERANCE_BITS);
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_int32)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::f32;
    auto output_type = element::i32;

    typedef float input_c_type;
    typedef int32_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_NEAREST_TOWARD_EVEN;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {2});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {1});
    auto quantize =
        make_shared<op::Quantize>(X, scale, offset, output_type, quantization_axes, round_mode);
    auto f = make_shared<Function>(quantize, ParameterVector{X});

    std::vector<input_c_type> x{0, -1, 2, -3, 4, -5, 6, -7, 8, -9, 10, -11};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {1, 1, 2, -1, 3, -1, 4, -3, 5, -3, 6, -5});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, dequantize_int32)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::i32;
    auto output_type = element::f32;

    typedef int32_t input_c_type;
    typedef float output_c_type;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(output_type, scale_offset_shape, {2});
    auto offset = op::Constant::create(input_type, scale_offset_shape, {1});
    auto dequantize = make_shared<op::Dequantize>(X, scale, offset, output_type, quantization_axes);
    auto f = make_shared<Function>(dequantize, ParameterVector{X});

    std::vector<input_c_type> x{1, 1, 2, -1, 3, -1, 4, -3, 5, -3, 6, -5};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {0, 0, 2, -4, 4, -4, 6, -8, 8, -8, 10, -12});
    test_case.run(MIN_FLOAT_TOLERANCE_BITS);
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_int32_zero_offset)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::f32;
    auto output_type = element::i32;

    typedef float input_c_type;
    typedef int32_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_NEAREST_TOWARD_EVEN;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {2});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {0});
    auto quantize =
        make_shared<op::Quantize>(X, scale, offset, output_type, quantization_axes, round_mode);
    auto f = make_shared<Function>(quantize, ParameterVector{X});

    std::vector<input_c_type> x{0, -1, 2, -3, 4, -5, 6, -7, 8, -9, 10, -11};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {0, 0, 1, -2, 2, -2, 3, -4, 4, -4, 5, -6});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, dequantize_int32_zero_offset)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::i32;
    auto output_type = element::f32;

    typedef int32_t input_c_type;
    typedef float output_c_type;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(output_type, scale_offset_shape, {2});
    auto offset = op::Constant::create(input_type, scale_offset_shape, {0});
    auto dequantize = make_shared<op::Dequantize>(X, scale, offset, output_type, quantization_axes);
    auto f = make_shared<Function>(dequantize, ParameterVector{X});

    std::vector<input_c_type> x{0, 0, 1, -2, 2, -2, 3, -4, 4, -4, 5, -6};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {0, 0, 2, -4, 4, -4, 6, -8, 8, -8, 10, -12});
    test_case.run(MIN_FLOAT_TOLERANCE_BITS);
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_clamp_uint8)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::f32;
    auto output_type = element::u8;

    typedef float input_c_type;
    typedef uint8_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_NEAREST_TOWARD_EVEN;

    auto max = std::numeric_limits<uint8_t>::max();

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {1.0 / (max + 1.0)});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {0});
    auto quantize =
        make_shared<op::Quantize>(X, scale, offset, output_type, quantization_axes, round_mode);
    auto f = make_shared<Function>(quantize, ParameterVector{X});

    std::vector<input_c_type> x{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(
        input_shape, {0, max, max, max, max, max, max, max, max, max, max, max});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_clamp_int8)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::f32;
    auto output_type = element::i8;

    typedef float input_c_type;
    typedef int8_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_NEAREST_TOWARD_EVEN;

    auto min = std::numeric_limits<int8_t>::min();
    auto max = std::numeric_limits<int8_t>::max();

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {1.0 / (max + 1.0)});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {0});
    auto quantize =
        make_shared<op::Quantize>(X, scale, offset, output_type, quantization_axes, round_mode);
    auto f = make_shared<Function>(quantize, ParameterVector{X});

    std::vector<input_c_type> x{0, -1, 2, -3, 4, -5, 6, -7, 8, -9, 10, -11};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(
        input_shape, {0, min, max, min, max, min, max, min, max, min, max, min});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_clamp_int32)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::f64;
    auto output_type = element::i32;

    // TODO: fails with input due to 32 bits
    typedef double input_c_type;
    typedef int32_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_NEAREST_TOWARD_EVEN;

    auto min = std::numeric_limits<int32_t>::min();
    auto max = std::numeric_limits<int32_t>::max();

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {1.0 / (max + 1.0)});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {0});
    auto quantize =
        make_shared<op::Quantize>(X, scale, offset, output_type, quantization_axes, round_mode);
    auto f = make_shared<Function>(quantize, ParameterVector{X});

    std::vector<input_c_type> x{0, -1, 2, -3, 4, -5, 6, -7, 8, -9, 10, -11};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(
        input_shape, {0, min, max, min, max, min, max, min, max, min, max, min});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_ROUND_NEAREST_TOWARD_ZERO)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::f32;
    auto output_type = element::i8;

    typedef float input_c_type;
    typedef int8_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_NEAREST_TOWARD_ZERO;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {4});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {0});
    auto quantize =
        make_shared<op::Quantize>(X, scale, offset, output_type, quantization_axes, round_mode);
    auto f = make_shared<Function>(quantize, ParameterVector{X});

    std::vector<input_c_type> x{9, 10, 11, -9, -10, -11, 13, 14, 15, -13, -14, -15};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {2, 2, 3, -2, -2, -3, 3, 3, 4, -3, -3, -4});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_ROUND_NEAREST_TOWARD_INFINITY)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::f32;
    auto output_type = element::i8;

    typedef float input_c_type;
    typedef int8_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_NEAREST_TOWARD_INFINITY;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {4});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {0});
    auto quantize =
        make_shared<op::Quantize>(X, scale, offset, output_type, quantization_axes, round_mode);
    auto f = make_shared<Function>(quantize, ParameterVector{X});

    std::vector<input_c_type> x{9, 10, 11, -9, -10, -11, 13, 14, 15, -13, -14, -15};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {2, 3, 3, -2, -3, -3, 3, 4, 4, -3, -4, -4});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_ROUND_NEAREST_UPWARD)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::f32;
    auto output_type = element::i8;

    typedef float input_c_type;
    typedef int8_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_NEAREST_UPWARD;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {4});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {0});
    auto quantize =
        make_shared<op::Quantize>(X, scale, offset, output_type, quantization_axes, round_mode);
    auto f = make_shared<Function>(quantize, ParameterVector{X});

    std::vector<input_c_type> x{9, 10, 11, -9, -10, -11, 13, 14, 15, -13, -14, -15};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {2, 3, 3, -2, -2, -3, 3, 4, 4, -3, -3, -4});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_ROUND_NEAREST_DOWNWARD)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::f32;
    auto output_type = element::i8;

    typedef float input_c_type;
    typedef int8_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_NEAREST_DOWNWARD;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {4});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {0});
    auto quantize =
        make_shared<op::Quantize>(X, scale, offset, output_type, quantization_axes, round_mode);
    auto f = make_shared<Function>(quantize, ParameterVector{X});

    std::vector<input_c_type> x{9, 10, 11, -9, -10, -11, 13, 14, 15, -13, -14, -15};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {2, 2, 3, -2, -3, -3, 3, 3, 4, -3, -4, -4});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_ROUND_NEAREST_TOWARD_EVEN)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::f32;
    auto output_type = element::i8;

    typedef float input_c_type;
    typedef int8_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_NEAREST_TOWARD_EVEN;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {4});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {0});
    auto quantize =
        make_shared<op::Quantize>(X, scale, offset, output_type, quantization_axes, round_mode);
    auto f = make_shared<Function>(quantize, ParameterVector{X});

    std::vector<input_c_type> x{9, 10, 11, -9, -10, -11, 13, 14, 15, -13, -14, -15};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {2, 2, 3, -2, -2, -3, 3, 4, 4, -3, -4, -4});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_ROUND_TOWARD_INFINITY)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::f32;
    auto output_type = element::i8;

    typedef float input_c_type;
    typedef int8_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_TOWARD_INFINITY;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {4});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {0});
    auto quantize = make_shared<op::Quantize>(
        X,
        scale,
        offset,
        output_type,
        quantization_axes,
        static_cast<op::Quantize::RoundMode>(static_cast<int>(round_mode)));
    auto f = make_shared<Function>(quantize, ParameterVector{X});

    std::vector<input_c_type> x{9, 10, 11, -9, -10, -11, 13, 14, 15, -13, -14, -15};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {3, 3, 3, -3, -3, -3, 4, 4, 4, -4, -4, -4});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_ROUND_TOWARD_ZERO)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::f32;
    auto output_type = element::i8;

    typedef float input_c_type;
    typedef int8_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_TOWARD_ZERO;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {4});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {0});
    auto quantize = make_shared<op::Quantize>(
        X,
        scale,
        offset,
        output_type,
        quantization_axes,
        static_cast<op::Quantize::RoundMode>(static_cast<int>(round_mode)));
    auto f = make_shared<Function>(quantize, ParameterVector{X});

    std::vector<input_c_type> x{9, 10, 11, -9, -10, -11, 13, 14, 15, -13, -14, -15};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {2, 2, 2, -2, -2, -2, 3, 3, 3, -3, -3, -3});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_ROUND_UP)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::f32;
    auto output_type = element::i8;

    typedef float input_c_type;
    typedef int8_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_UP;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {4});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {0});
    auto quantize =
        make_shared<op::Quantize>(X, scale, offset, output_type, quantization_axes, round_mode);
    auto f = make_shared<Function>(quantize, ParameterVector{X});

    std::vector<input_c_type> x{9, 10, 11, -9, -10, -11, 13, 14, 15, -13, -14, -15};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {3, 3, 3, -2, -2, -2, 4, 4, 4, -3, -3, -3});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_ROUND_DOWN)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape;
    AxisSet quantization_axes;

    auto input_type = element::f32;
    auto output_type = element::i8;

    typedef float input_c_type;
    typedef int8_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_DOWN;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = op::Constant::create(input_type, scale_offset_shape, {4});
    auto offset = op::Constant::create(output_type, scale_offset_shape, {0});
    auto quantize =
        make_shared<op::Quantize>(X, scale, offset, output_type, quantization_axes, round_mode);
    auto f = make_shared<Function>(quantize, ParameterVector{X});

    std::vector<input_c_type> x{9, 10, 11, -9, -10, -11, 13, 14, 15, -13, -14, -15};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_expected_output<output_c_type>(input_shape,
                                                 {2, 2, 2, -3, -3, -3, 3, 3, 3, -4, -4, -4});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, dequantize_dynamic_offset)
{
    Shape input_shape{4};
    Shape scale_offset_shape = {};
    AxisSet quantization_axes;

    auto input_type = element::u8;
    auto output_type = element::f32;

    typedef uint8_t input_c_type;
    typedef float output_c_type;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = make_shared<op::Parameter>(output_type, scale_offset_shape);
    auto offset = make_shared<op::Parameter>(input_type, scale_offset_shape);
    auto dequantize = make_shared<op::Dequantize>(X, scale, offset, output_type, quantization_axes);
    auto f = make_shared<Function>(dequantize, ParameterVector{X, scale, offset});

    std::vector<input_c_type> x{0, 3, 128, 255};
    std::vector<output_c_type> Scale{2};
    std::vector<input_c_type> Offset{128};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_input<output_c_type>({Scale});
    test_case.add_input<input_c_type>({Offset});

    test_case.add_expected_output<output_c_type>(input_shape, {-256.0f, -250.0f, 0.0f, 254.0f});
    test_case.run(MIN_FLOAT_TOLERANCE_BITS);
}

NGRAPH_TEST(${BACKEND_NAME}, quantize_dynamic_offset)
{
    Shape input_shape{4, 3};
    Shape scale_offset_shape = {};
    AxisSet quantization_axes;

    auto input_type = element::f32;
    auto output_type = element::u8;

    typedef float input_c_type;
    typedef uint8_t output_c_type;

    op::Quantize::RoundMode round_mode = op::Quantize::RoundMode::ROUND_NEAREST_TOWARD_EVEN;

    auto X = make_shared<op::Parameter>(input_type, input_shape);
    auto scale = make_shared<op::Parameter>(input_type, scale_offset_shape);
    auto offset = make_shared<op::Parameter>(output_type, scale_offset_shape);
    auto quantize =
        make_shared<op::Quantize>(X, scale, offset, output_type, quantization_axes, round_mode);
    auto f = make_shared<Function>(quantize, ParameterVector{X, scale, offset});

    std::vector<input_c_type> x{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    std::vector<input_c_type> Scale{2};
    std::vector<output_c_type> Offset{1};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<input_c_type>({x});
    test_case.add_input<input_c_type>({Scale});
    test_case.add_input<output_c_type>({Offset});

    test_case.add_expected_output<output_c_type>(input_shape, {1, 1, 2, 3, 3, 3, 4, 5, 5, 5, 6, 7});
    test_case.run();
}
