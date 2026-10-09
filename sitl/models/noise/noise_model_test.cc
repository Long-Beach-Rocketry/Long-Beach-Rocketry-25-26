#include "noise_model.h"

#include <limits>
#include <stdexcept>

#include <gtest/gtest.h>

using namespace std;

namespace LBR
{
namespace
{

TEST(NoiseModelTest, ZeroErrorsPreserveTheTrueValue)
{
    NoiseModel model({});
    EXPECT_DOUBLE_EQ(model.sample(101325.0, 0.0), 101325.0);
    EXPECT_DOUBLE_EQ(model.sample(-12.5, 7.0), -12.5);
}

TEST(NoiseModelTest, FixedBiasAndDriftTrackAbsoluteSimulationTime)
{
    NoiseModelConfig config;
    config.fixed_bias = 2.0;
    config.bias_drift_per_s = -0.25;
    NoiseModel model(config);

    EXPECT_DOUBLE_EQ(model.sample(100.0, 0.0), 102.0);
    EXPECT_DOUBLE_EQ(model.sample(100.0, 4.0), 101.0);
    EXPECT_DOUBLE_EQ(model.sample(100.0, 4.0), 101.0);
}

TEST(NoiseModelTest, QuantizationRoundsTheBiasedReading)
{
    NoiseModelConfig config;
    config.fixed_bias = 0.1;
    config.quantization_step = 0.25;
    NoiseModel model(config);

    EXPECT_DOUBLE_EQ(model.sample(10.1, 0.0), 10.25);
    EXPECT_DOUBLE_EQ(model.sample(-1.2, 0.0), -1.0);
}

TEST(NoiseModelTest, JitterIsRepeatableForTheSameSeed)
{
    NoiseModelConfig config;
    config.jitter_stddev = 0.5;
    NoiseModel first(config, 123U);
    NoiseModel second(config, 123U);

    for (int sample_index = 0; sample_index < 8; ++sample_index)
    {
        EXPECT_DOUBLE_EQ(first.sample(50.0, sample_index),
                         second.sample(50.0, sample_index));
    }
    EXPECT_NE(first.sample(50.0, 8.0), 50.0);
}

TEST(NoiseModelTest, RejectsInvalidConfigurationAndInputs)
{
    NoiseModelConfig config;
    config.quantization_step = -0.1;
    EXPECT_THROW(NoiseModel invalid(config), invalid_argument);

    NoiseModel model({});
    EXPECT_THROW((void)model.sample(1.0, -0.1), invalid_argument);
    EXPECT_THROW(
        (void)model.sample(numeric_limits<double>::quiet_NaN(), 0.0),
        invalid_argument);
}

}  // namespace
}  // namespace LBR
