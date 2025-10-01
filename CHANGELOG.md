# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [2.0.0]

### Added
- Introduced own namespace

### Changed
- Updated UPT Core to 0.9
- Use sensirion::upt namespace

## [1.0.0]

### Added
- Add support for STCC4

### Changed
- Refactored auto detection & sensor list

## [0.3.3]

### Fixed
- SFA3x driver integration with driver version 1.0.0

## [0.3.2]

### Fixed
- Driver dependecies are now fixed
- SEN66 and SCD4x serial compilation error fixed

## [0.3.1]

### Fixed
- sensorName was undefined but used in logging.

## [0.3.0]

### Added

- Add support for SEN66

## [0.2.0]

### Added

- Initialization of the STC3x RH value at 50% to achieve more reliable results.

### Fixed

- Fixed memory leak in I2C scanning code

## [0.1.2]

### Fixed

- Sensor wrappers have been updated to support the latest drivers

## [0.1.1]

### Modified

- State Machine determines Sensor MetaData during Sensor initialization

## [0.1.0]

### Added

- Automatic sensor detection & data readout for SCD4X, SCD30, SEN5X, SFA3X, SGP4X, SHT4X, STC3X and SVM4X sensors
