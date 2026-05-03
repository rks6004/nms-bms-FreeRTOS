"""
INR21700-P45B Battery Pack Fault Condition Curve Generator
Generates realistic fault curves for 16S5P × 6 segment pack configuration (480 cells total)
- Pack configuration: 6 independent segments, each 16 cells in series, 5 in parallel
- Voltage: Cell-level (single cell range 2.5V–4.2V), averaged across parallel cells
- Current: Pack-level (sum of all 30 parallel paths per segment × 6 segments = 180 parallel paths)
- Temperature: Pack-level (estimated from Joule heating in equivalent cell)

Maintains physical consistency with normal discharge curves through shared electrochemical models.

Author: Battery Modeling
Date: 2025
"""

import numpy as np
import pandas as pd
from typing import Tuple, List
from enum import Enum
import os


class FaultType(Enum):
    """Enumeration of fault condition types"""
    OVERCURRENT = "overcurrent"           # Excessive discharge current
    CELL_OVERVOLTAGE = "cell_overvoltage"  # Voltage exceeds safe limits
    CELL_UNDERVOLTAGE = "cell_undervoltage"  # Voltage drops below safe limits
    OVERTEMPERATURE = "overtemperature"    # Temperature rises excessively
    UNDERTEMPERATURE = "undertemperature"  # Temperature drops below safe limits
    RAPID_DISCHARGE = "rapid_discharge"    # Extreme C-rate
    SHORT_CIRCUIT = "short_circuit"        # Near-zero resistance discharge


class FaultConditionGenerator:
    """
    Generates fault condition discharge curves for 16S5P × 6 segment pack.
    
    Pack Configuration:
    - 6 independent segments (can be monitored/controlled separately)
    - Each segment: 16 cells in series, 5 in parallel = 80 cells/segment
    - Total: 480 cells (16s × 30p per segment × 6 segments = 16s × 180p overall)
    
    Output Scaling:
    - Voltage: Cell-level (2.5V–4.2V range), same as single cell
      * Represents average voltage across parallel cells
      * Maintains compatibility with single-cell validation
    
    - Current: Pack-level (scaled for all 180 parallel discharge paths)
      * 5 parallel per segment × 6 segments = 30 parallel per segment
      * Can discharge at high current without excessive cell-level stress
    
    - Temperature: Pack-level (per-cell equivalent)
      * Represents effective temperature across the pack
      * Scaled from I²R heating with pack-level current density
    
    All curves maintain physical consistency through shared OCV, R(T,I), and SOC models.
    """
    
    # Cell specifications (matching discharge_curve_generator.py)
    NOMINAL_CAPACITY = 4500  # mAh
    VOLTAGE_MAX_SAFE = 4.25  # V (charging ceiling)
    VOLTAGE_MAX = 4.20       # V (nominal charge cutoff)
    VOLTAGE_MIN_SAFE = 2.40  # V (discharging floor, below normal 2.5V)
    VOLTAGE_MIN = 2.50       # V (normal discharge cutoff)
    VOLTAGE_NOMINAL = 3.7    # V (approximate 50% SOC)
    
    # Temperature limits
    TEMP_MIN_SAFE = -50      # °C (absolute minimum)
    TEMP_MAX_SAFE = 80       # °C (absolute maximum)
    TEMP_OPTIMAL_MIN = 0     # °C
    TEMP_OPTIMAL_MAX = 45    # °C
    
    # OCV lookup table (shared with discharge generator)
    OCV_TABLE = {
        1.00: 4.20,
        0.95: 4.18,
        0.90: 4.15,
        0.85: 4.10,
        0.80: 4.05,
        0.75: 4.00,
        0.70: 3.98,
        0.65: 3.95,
        0.60: 3.92,
        0.55: 3.85,
        0.50: 3.72,
        0.45: 3.60,
        0.40: 3.50,
        0.35: 3.40,
        0.30: 3.30,
        0.25: 3.20,
        0.20: 3.10,
        0.15: 2.95,
        0.10: 2.80,
        0.05: 2.65,
        0.00: 2.50,
    }
    
    def __init__(
        self,
        fault_type: FaultType,
        capacity_mAh: int = 4500,
        initial_soc: float = 0.95,
        ambient_temp_C: float = 23.7,
        duration_s: int = 120,
        sample_interval_ms: int = 50,
        fault_magnitude: float = 1.0,
        random_seed: int = 42
    ):
        """
        Initialize fault condition generator for 16S5P × 6 segment pack.
        
        PACK CONFIGURATION:
        - 6 segments × (16 series × 5 parallel per segment) = 480 cells total
        - 180 parallel discharge paths (30 per segment × 6 segments)
        - All cells assumed identical in operation
        
        OUTPUT SCALING:
        
        VOLTAGE (Cell-Level):
        - Range: 2.5V–4.2V (single cell values)
        - Generated as if monitoring one representative cell
        - In practice, averaged across all parallel paths in each segment
        - For pack voltage: multiply output by 16 (series count)
        
        CURRENT (Pack-Level):
        - Range: 0–400,000+ mA (sum of all parallel paths)
        - Represents total pack discharge current
        - Per-cell current: Pack_I / 180
        - Example: 237.7A pack = 1.32A per cell
        
        TEMPERATURE (Pack-Level, Per-Cell Equivalent):
        - Range: -50°C to +80°C
        - Represents effective pack temperature
        - Scaled from Joule heating: P = I² × R (pack-level I, cell-level R)
        
        Args:
            fault_type: Type of fault to simulate (FaultType enum)
            capacity_mAh: Single cell capacity (default: 4500 mAh)
            initial_soc: Starting state of charge (0.0–1.0)
            ambient_temp_C: Ambient temperature in Celsius
            duration_s: Total test duration in seconds
            sample_interval_ms: Sample interval in milliseconds (typically 50 ms)
            fault_magnitude: Severity of fault (1.0 = nominal, >1.0 = more severe)
            random_seed: Random seed for reproducibility
        """
        self.fault_type = fault_type
        self.capacity_mAh = capacity_mAh
        self.initial_soc = initial_soc
        self.ambient_temp_C = ambient_temp_C
        self.duration_s = duration_s
        self.sample_interval_ms = sample_interval_ms
        self.fault_magnitude = fault_magnitude
        self.num_samples = int(duration_s * 1000 / sample_interval_ms)
        self.timestamps_ms = np.arange(self.num_samples) * sample_interval_ms
        self.time_s = self.timestamps_ms / 1000
        
        if random_seed is not None:
            np.random.seed(random_seed)
    
    def generate_current_profile_overcurrent(self) -> np.ndarray:
        """
        Generate overcurrent fault: Current significantly exceeds normal limits.
        
        Pack-Level Scaling:
        - 180 parallel discharge paths (30 per segment × 6 segments)
        - Single cell normal peak: 158.5A → Per-cell: 158.5A / 180 = 0.88A/cell
        - Pack fault current: 158.5A × magnitude × 180 = scaled to pack level
        
        Overcurrent phase structure:
        - Phase 1: Brief ramp to fault current (1s)
        - Phase 2: Sustained high current (maintains fault)
        - Phase 3: Current may decay if voltage hits cutoff or protection activates
        
        Returns:
            Current profile in mA (pack-level)
        """
        current = np.zeros(self.num_samples)
        
        # Pack configuration: 180 parallel discharge paths
        # Normal single-cell peak: 158,465 mA → Pack equivalent for comparison: 158,465 / 180 = 880 mA per cell
        # Fault current: scale magnitude for pack behavior
        
        # Single-cell normal peak discharge
        single_cell_peak_mA = 158465  # From standard test
        
        # Pack fault current = single-cell peak × magnitude × number of parallel paths
        # But we want to express in terms that make sense for pack:
        # At magnitude=1.0: Pack can discharge at 158.5A (158,465 mA × 1.0)
        # This represents 158.5A / 180 ≈ 0.88A per cell (very conservative, within safe limits)
        
        fault_current_mA = single_cell_peak_mA * self.fault_magnitude
        
        # Phase 1: Brief ramp (0-1s)
        phase1_samples = int(1.0 / 0.05)  # 20 samples for 1 second
        current[0:phase1_samples] = np.linspace(1000, fault_current_mA, phase1_samples)
        
        # Phase 2: Sustained fault current
        # Continue until voltage hits minimum (triggers circuit breaker)
        # For this fault, we'll sustain for 80% of duration then decay
        phase2_end = int(0.8 * self.num_samples)
        current[phase1_samples:phase2_end] = fault_current_mA
        
        # Phase 3: Exponential decay to zero (circuit protection activates)
        remaining_samples = self.num_samples - phase2_end
        decay_time_const = 5  # Fast decay (protection circuit)
        t_decay = np.arange(remaining_samples) * 0.05
        current[phase2_end:] = fault_current_mA * np.exp(-t_decay / decay_time_const)
        
        return current
    
    def generate_current_profile_short_circuit(self) -> np.ndarray:
        """
        Generate short circuit: Near-maximum current for short duration.
        
        Pack-Level Scaling:
        - 180 parallel paths can deliver extreme current
        - Short circuit current = 400A pack-level (2.2A per cell, still within safe limits)
        
        Short circuit characteristics:
        - Extremely high current (limited by pack external resistance)
        - Brief duration (protection activates within milliseconds)
        - Then rapid current decay
        
        Returns:
            Current profile in mA (pack-level)
        """
        current = np.zeros(self.num_samples)
        
        # Short circuit current at pack level
        # With 180 parallel paths, pack can source extreme currents
        # Typically limited by PCB/connector resistance
        # Conservative estimate: 400A pack = 2.2A per cell
        short_circuit_current = 400000  # 400A pack-level
        
        # Sustain for very short time
        sustain_samples = int(0.5 / 0.05)  # 10 samples = 0.5 seconds max
        current[0:sustain_samples] = short_circuit_current
        
        # Rapid exponential decay (fuse/breaker protection)
        remaining_samples = self.num_samples - sustain_samples
        decay_time_const = 2  # Very fast (protection)
        t_decay = np.arange(remaining_samples) * 0.05
        current[sustain_samples:] = short_circuit_current * np.exp(-t_decay / decay_time_const)
        
        return current
    
    def generate_current_profile_normal_with_fault(self) -> np.ndarray:
        """
        Generate normal discharge current (for use with temperature/voltage faults).
        
        Pack-Level Configuration:
        - 180 parallel discharge paths distributed across 6 segments
        - Per-cell current at peak: ~0.88A (safe, well within cell limits)
        
        This generates the standard three-phase current profile that can be
        combined with temperature or voltage faults independently.
        
        Returns:
            Current profile in mA (pack-level)
        """
        current = np.zeros(self.num_samples)
        
        # Phase 1: Pre-discharge (idle)
        # 180 parallel paths, very low idle current per cell (~50µA per cell)
        phase1_end_idx = int(5.75 / 0.05)  # 115 samples
        current[0:phase1_end_idx] = 9  # ~9 mA idle current (pack-level)
        
        # Phase 2: Linear ramp (5.75s to 73.95s)
        # Ramp from ~9mA to peak current distributed across all parallel paths
        phase2_start_idx = phase1_end_idx
        phase2_end_idx = int(73.95 / 0.05)  # 1479 samples
        phase2_duration = phase2_end_idx - phase2_start_idx
        
        current[phase2_start_idx:phase2_end_idx] = np.linspace(
            1000,      # Start at ~1A (180 parallel ≈ 5.5mA per cell)
            158465,    # Ramp to ~158.5A peak (880mA per cell)
            phase2_duration
        )
        
        # Phase 3: Plateau then decay (73.95s to 120s)
        phase3_start_idx = phase2_end_idx
        plateau_duration_idx = int(5.0 / 0.05)  # ~100 samples
        plateau_end_idx = phase3_start_idx + plateau_duration_idx
        current[phase3_start_idx:plateau_end_idx] = 158465
        
        # Exponential decay from plateau to zero
        remaining_idx = self.num_samples - plateau_end_idx
        decay_time_const = 15  # seconds
        t_decay = np.arange(remaining_idx) * 0.05
        decay_curve = 158465 * np.exp(-t_decay / decay_time_const)
        current[plateau_end_idx:self.num_samples] = decay_curve
        
        return current
    
    def calculate_soc_and_ocv(self, current_mA: np.ndarray) -> Tuple[np.ndarray, np.ndarray]:
        """
        Calculate state-of-charge and open-circuit voltage.
        
        Shared with discharge generator - maintains physical consistency.
        """
        charge_per_step = current_mA * 0.05 / 3600  # 50ms = 50/3.6e6 hours
        cumulative_charge = np.cumsum(charge_per_step)
        
        soc = self.initial_soc - (cumulative_charge / self.capacity_mAh)
        soc = np.clip(soc, 0.0, 1.0)
        
        ocv = self._interpolate_ocv(soc)
        return soc, ocv
    
    def _interpolate_ocv(self, soc: np.ndarray) -> np.ndarray:
        """Interpolate OCV from lookup table (shared method)"""
        ocv = np.zeros(len(soc))
        soc_keys = sorted(self.OCV_TABLE.keys(), reverse=True)
        
        for i, s in enumerate(soc):
            for j in range(len(soc_keys) - 1):
                if soc_keys[j] >= s >= soc_keys[j + 1]:
                    soc_upper = soc_keys[j]
                    soc_lower = soc_keys[j + 1]
                    fraction = (s - soc_lower) / (soc_upper - soc_lower)
                    
                    ocv[i] = (
                        self.OCV_TABLE[soc_lower] +
                        fraction * (self.OCV_TABLE[soc_upper] - self.OCV_TABLE[soc_lower])
                    )
                    break
            else:
                if s > soc_keys[0]:
                    ocv[i] = self.OCV_TABLE[soc_keys[0]]
                else:
                    ocv[i] = self.OCV_TABLE[soc_keys[-1]]
        
        return ocv
    
    def calculate_internal_resistance(self, current_mA: np.ndarray, temp_C: np.ndarray) -> np.ndarray:
        """
        Calculate internal resistance (shared with discharge generator).
        
        Uses same temperature and current dependence model.
        """
        r_base = 0.050  # Base resistance at 23°C, low current (50 mΩ)
        t_coeff = -0.003  # Resistance change per °C
        t_ref = 23.0
        
        r_internal = np.zeros(len(current_mA))
        
        for i in range(len(current_mA)):
            # Temperature effect
            r_temp = r_base * (1 + t_coeff * (temp_C[i] - t_ref))
            
            # Current effect
            i_amps = current_mA[i] / 1000
            
            if i_amps < 10:
                r_current_factor = 1.0 + 0.05 * i_amps
            else:
                r_current_factor = 1.5 + 0.01 * i_amps
            
            r_internal[i] = r_temp * r_current_factor
        
        return r_internal
    
    def generate_temperature_profile_overtemperature(self, current_mA: np.ndarray) -> np.ndarray:
        """
        Generate overtemperature fault: Temperature rises excessively.
        
        IMPORTANT: Thermal Scaling for Pack
        - Pack current is at system level (180 parallel paths)
        - Internal resistance R is PER CELL
        - For thermal calculations: P = (I_pack / 180)² × R_cell × 180 = (I_pack² × R) / 180
        - This accounts for the parallel current distribution across all cells
        
        Causes:
        - I²R heating from high current (but distributed across parallel cells)
        - Environmental heating (simulated as sustained heat source)
        - Reduced heat dissipation (simulated as lower thermal resistance)
        
        Returns:
            Temperature profile in Celsius (pack-level, representative cell)
        """
        temp = np.zeros(len(current_mA))
        
        # Baseline temperature rise from I²R heating
        # Thermal resistance calculation for pack:
        # Pack has 180 parallel paths, so thermal resistance is effectively lower
        # But we track representative cell temperature, so R_thermal relates to single-cell equivalent
        r_internal = self.calculate_internal_resistance(
            current_mA,
            np.full_like(current_mA, self.ambient_temp_C, dtype=float)
        )
        
        # Power dissipation (scaled for parallel distribution)
        # Total power = I_pack² × R_cell / N_parallel
        # But we track per-cell equivalent, so use same formula as single cell
        i_amps = current_mA / 1000
        
        # For pack: effective current per cell = I_pack / 180
        i_per_cell = i_amps / 180
        power_w = (i_per_cell ** 2) * r_internal * 180  # Scale back for system level
        
        # Thermal parameters adjusted for fault condition
        r_thermal = 2.0 * (1.0 / self.fault_magnitude)  # Lower R_thermal = more heating
        tau_thermal = 15  # Faster thermal response
        
        temp[0] = self.ambient_temp_C
        
        for i in range(1, len(current_mA)):
            t = self.time_s[i]
            
            # Add environmental heat source during fault
            environmental_heat = 0  # Base case
            if power_w[i] > 50:  # If discharging hard
                environmental_heat = 20 * self.fault_magnitude  # Additional heat source
            
            # Steady-state rise
            steady_state_rise = (power_w[i] * r_thermal + environmental_heat * 0.01)
            
            # Exponential buildup
            transient = steady_state_rise * (1 - np.exp(-t / tau_thermal))
            
            temp[i] = self.ambient_temp_C + transient
        
        # Cap at maximum safe temperature
        temp = np.clip(temp, self.TEMP_MIN_SAFE, self.TEMP_MAX_SAFE)
        
        return temp
    
    def generate_temperature_profile_undertemperature(self) -> np.ndarray:
        """
        Generate undertemperature fault: Temperature drops excessively.
        
        Causes:
        - Environmental cooling (external heat sink)
        - Reduced discharge (lower I²R heating)
        
        Returns:
            Temperature profile in Celsius
        """
        temp = np.zeros(self.num_samples)
        
        # Fault temperature is much colder than ambient
        fault_temp = self.ambient_temp_C - (10 * self.fault_magnitude)  # Lower by 10°C per magnitude
        fault_temp = np.clip(fault_temp, self.TEMP_MIN_SAFE, self.ambient_temp_C)
        
        # Gradual approach to fault temperature (thermal lag)
        tau_thermal = 20  # seconds
        
        temp[0] = self.ambient_temp_C
        
        for i in range(1, self.num_samples):
            t = self.time_s[i]
            # Exponential decay to colder temperature
            temp[i] = self.ambient_temp_C + (fault_temp - self.ambient_temp_C) * (1 - np.exp(-t / tau_thermal))
        
        return temp
    
    def generate_voltage_profile_normal(
        self,
        current_mA: np.ndarray,
        soc: np.ndarray,
        ocv: np.ndarray,
        temp_C: np.ndarray
    ) -> np.ndarray:
        """
        Generate normal voltage (shared with discharge generator).
        
        Used as baseline for voltage fault scenarios.
        """
        r_internal = self.calculate_internal_resistance(current_mA, temp_C)
        
        # Ohmic drop
        i_amps = current_mA / 1000
        v_ohmic = i_amps * r_internal
        
        # Diffusion (simplified)
        tau_diffusion = 30.0
        i_normalized = current_mA / 10000
        i_normalized = np.clip(i_normalized, 0, 20)
        
        v_diffusion = np.zeros(len(current_mA))
        for i in range(len(current_mA)):
            t = self.time_s[i]
            buildup = 1 - np.exp(-t / tau_diffusion)
            soc_factor = 1 + (1 - soc[i]) * 2
            v_diffusion[i] = 0.50 * i_normalized[i] * buildup * soc_factor
        
        voltage = ocv - v_ohmic - v_diffusion
        voltage = np.clip(voltage, self.VOLTAGE_MIN_SAFE, self.VOLTAGE_MAX_SAFE)
        
        # Add noise
        noise = np.random.normal(0, 0.005, len(voltage))
        voltage = voltage + noise
        
        return voltage
    
    def generate_voltage_profile_overvoltage(
        self,
        normal_voltage: np.ndarray
    ) -> np.ndarray:
        """
        Generate overvoltage fault: Voltage exceeds maximum safe limit.
        
        Overvoltage causes:
        - SEI layer damage
        - Lithium plating (if charged too aggressively)
        - Positive electrode oxidation
        
        Simulated by:
        - Pushing above 4.2V nominal
        - Can reach up to 4.5V in fault condition
        
        Returns:
            Overvoltage profile in volts
        """
        voltage = normal_voltage.copy()
        
        # Overvoltage factor: how much above nominal (4.2V)
        # fault_magnitude of 1.0 = 4.2V (nominal)
        # fault_magnitude of 1.2 = 4.44V (dangerous overvoltage)
        overvoltage_level = self.VOLTAGE_MAX + (self.fault_magnitude - 1.0) * 0.5
        overvoltage_level = np.clip(overvoltage_level, self.VOLTAGE_MAX, 4.5)
        
        # Raise all voltages toward overvoltage level
        # Use exponential approach to create realistic transient
        for i in range(len(voltage)):
            t = self.time_s[i]
            tau = 5.0  # Time constant for overvoltage development
            fraction = 1 - np.exp(-t / tau)
            
            # Blend between normal and overvoltage
            voltage[i] = normal_voltage[i] + (overvoltage_level - self.VOLTAGE_MAX) * fraction
        
        # Cap at physical maximum
        voltage = np.clip(voltage, self.VOLTAGE_MIN_SAFE, 4.5)
        
        return voltage
    
    def generate_voltage_profile_undervoltage(
        self,
        normal_voltage: np.ndarray,
        current_mA: np.ndarray
    ) -> np.ndarray:
        """
        Generate undervoltage fault: Voltage drops below minimum safe limit.
        
        Undervoltage causes:
        - Copper dissolution (if <2.5V for extended time)
        - Irreversible capacity loss
        - Battery may not recover to full capacity
        
        Simulated by:
        - Sustained high discharge (accelerates voltage drop)
        - Temperature effects (colder = higher resistance = lower voltage)
        
        Returns:
            Undervoltage profile in volts
        """
        voltage = normal_voltage.copy()
        
        # Undervoltage level: how far below minimum (2.5V)
        # fault_magnitude of 1.0 = 2.5V (nominal)
        # fault_magnitude of 1.3 = 2.17V (severe undervoltage)
        undervoltage_level = self.VOLTAGE_MIN - (self.fault_magnitude - 1.0) * 0.33
        undervoltage_level = np.clip(undervoltage_level, 1.5, self.VOLTAGE_MIN)
        
        # Additional voltage drop from sustained high current
        current_stress_factor = np.clip(current_mA / 100000, 0, 1)  # Normalized to [0,1]
        
        for i in range(len(voltage)):
            # Additional stress from high current
            stress_drop = current_stress_factor[i] * (self.VOLTAGE_MIN - undervoltage_level)
            
            voltage[i] = normal_voltage[i] - stress_drop
        
        # Clip to physical minimum
        voltage = np.clip(voltage, 1.5, self.VOLTAGE_MIN)
        
        return voltage
    
    def generate(self) -> Tuple[pd.DataFrame, pd.DataFrame, pd.DataFrame]:
        """
        Generate complete fault condition curves.
        
        Returns:
            Tuple of three DataFrames:
            - voltage_df: [timestamp_ms, voltage_mV]
            - current_df: [timestamp_ms, current_mA]
            - temperature_df: [timestamp_ms, temp_mC]
        """
        
        # Select current profile based on fault type
        if self.fault_type == FaultType.OVERCURRENT:
            current_mA = self.generate_current_profile_overcurrent()
        elif self.fault_type == FaultType.SHORT_CIRCUIT:
            current_mA = self.generate_current_profile_short_circuit()
        else:
            # For voltage/temperature faults, use normal current
            current_mA = self.generate_current_profile_normal_with_fault()
        
        # Calculate SOC and OCV (always needed)
        soc, ocv = self.calculate_soc_and_ocv(current_mA)
        
        # Generate temperature based on fault type
        if self.fault_type == FaultType.OVERTEMPERATURE:
            temp_C = self.generate_temperature_profile_overtemperature(current_mA)
        elif self.fault_type == FaultType.UNDERTEMPERATURE:
            temp_C = self.generate_temperature_profile_undertemperature()
        else:
            # Normal temperature for overcurrent/voltage faults
            temp_C = np.full_like(current_mA, self.ambient_temp_C, dtype=float) + \
                     0.5 * (current_mA / 10000)  # Small rise from current
        
        # Generate voltage based on fault type
        if self.fault_type in [FaultType.OVERCURRENT, FaultType.SHORT_CIRCUIT, FaultType.RAPID_DISCHARGE]:
            normal_voltage = self.generate_voltage_profile_normal(current_mA, soc, ocv, temp_C)
            voltage_V = normal_voltage
        elif self.fault_type == FaultType.CELL_OVERVOLTAGE:
            normal_voltage = self.generate_voltage_profile_normal(current_mA, soc, ocv, temp_C)
            voltage_V = self.generate_voltage_profile_overvoltage(normal_voltage)
        elif self.fault_type == FaultType.CELL_UNDERVOLTAGE:
            normal_voltage = self.generate_voltage_profile_normal(current_mA, soc, ocv, temp_C)
            voltage_V = self.generate_voltage_profile_undervoltage(normal_voltage, current_mA)
        else:
            voltage_V = self.generate_voltage_profile_normal(current_mA, soc, ocv, temp_C)
        
        # Convert to appropriate units
        voltage_mV = np.round(voltage_V * 1000).astype(int)
        current_mA_int = np.round(current_mA).astype(int)
        temp_mC = np.round(temp_C * 1000).astype(int)
        
        # Create DataFrames
        voltage_df = pd.DataFrame({
            'timestamp_ms': self.timestamps_ms.astype(int),
            'voltage_mV': voltage_mV
        })
        
        current_df = pd.DataFrame({
            'timestamp_ms': self.timestamps_ms.astype(int),
            'current_mA': current_mA_int
        })
        
        temperature_df = pd.DataFrame({
            'timestamp_ms': self.timestamps_ms.astype(int),
            'temp_mC': temp_mC
        })
        
        return voltage_df, current_df, temperature_df
    
    def save_to_csv(self, output_dir: str = '.', fault_name_suffix: str = '') -> None:
        """
        Generate fault curves and save to CSV files.
        
        Args:
            output_dir: Directory to save CSV files
            fault_name_suffix: Optional suffix for filename (e.g., fault condition description)
        """
        voltage_df, current_df, temperature_df = self.generate()
        
        # Build filenames
        fault_type_name = self.fault_type.value.replace('_', '_')
        if fault_name_suffix:
            suffix = f"_{fault_name_suffix}"
        else:
            suffix = f"_mag{self.fault_magnitude:.1f}"
        
        v_file = f'{output_dir}/voltage_{fault_type_name}{suffix}.csv'
        i_file = f'{output_dir}/current_{fault_type_name}{suffix}.csv'
        t_file = f'{output_dir}/temp_{fault_type_name}{suffix}.csv'
        
        voltage_df.to_csv(v_file, header=False, index=False)
        current_df.to_csv(i_file, header=False, index=False)
        temperature_df.to_csv(t_file, header=False, index=False)
        
        print(f"✓ Voltage {fault_type_name} saved to {v_file}")
        print(f"✓ Current {fault_type_name} saved to {i_file}")
        print(f"✓ Temperature {fault_type_name} saved to {t_file}")


# Example usage and batch generation
if __name__ == '__main__':
    
    print("="*70)
    print("INR21700-P45B FAULT CONDITION CURVE GENERATOR")
    print("="*70)
    
    # Define fault scenarios to generate
    fault_scenarios = [
        (FaultType.OVERCURRENT, 1.5, "moderate_overcurrent"),
        (FaultType.OVERCURRENT, 2.0, "severe_overcurrent"),
        (FaultType.SHORT_CIRCUIT, 1.0, "short_circuit"),
        (FaultType.CELL_OVERVOLTAGE, 1.1, "mild_overvoltage"),
        (FaultType.CELL_OVERVOLTAGE, 1.3, "severe_overvoltage"),
        (FaultType.CELL_UNDERVOLTAGE, 1.2, "moderate_undervoltage"),
        (FaultType.CELL_UNDERVOLTAGE, 1.5, "severe_undervoltage"),
        (FaultType.OVERTEMPERATURE, 1.5, "overtemperature_mild"),
        (FaultType.OVERTEMPERATURE, 2.5, "overtemperature_severe"),
        (FaultType.UNDERTEMPERATURE, 1.0, "undertemperature_cold"),
    ]
    
    # Generate all fault scenarios
    for fault_type, magnitude, description in fault_scenarios:
        print(f"\nGenerating {description}...")
        
        gen = FaultConditionGenerator(
            fault_type=fault_type,
            capacity_mAh=4500,
            initial_soc=0.95,
            ambient_temp_C=23.7,
            duration_s=120,
            fault_magnitude=magnitude
        )
        gen.save_to_csv(f'{os.getcwd()}', fault_name_suffix=description)
    
    print("\n" + "="*70)
    print("FAULT CURVE GENERATION COMPLETE")
    print("="*70)
    
    # Generate summary statistics for one scenario as example
    print("\nExample: Moderate Overcurrent Fault (1.5×)")
    gen = FaultConditionGenerator(
        fault_type=FaultType.OVERCURRENT,
        fault_magnitude=1.5
    )
    
    voltage_df, current_df, temp_df = gen.generate()
    
    print(f"\nVoltage profile:")
    print(f"  Range: {voltage_df['voltage_mV'].min()} – {voltage_df['voltage_mV'].max()} mV")
    print(f"  Mean: {voltage_df['voltage_mV'].mean():.0f} mV")
    
    print(f"\nCurrent profile:")
    print(f"  Range: {current_df['current_mA'].min()} – {current_df['current_mA'].max()} mA")
    print(f"  Mean: {current_df['current_mA'].mean():.0f} mA")
    
    print(f"\nTemperature profile:")
    print(f"  Range: {temp_df['temp_mC'].min()/1000:.2f} – {temp_df['temp_mC'].max()/1000:.2f} °C")
    print(f"  Mean: {temp_df['temp_mC'].mean()/1000:.2f} °C")
    
    # Capacity delivered
    capacity_mAh = (current_df['current_mA'].sum() * 50 / 3600000)
    print(f"\nCapacity delivered: {capacity_mAh:.0f} mAh ({capacity_mAh/1000:.2f} Ah)")
