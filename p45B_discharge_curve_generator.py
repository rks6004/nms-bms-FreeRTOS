"""
INR21700-P45B Discharge Curve Generator
Generates realistic voltage, current, and temperature discharge traces
matching the Molicell datasheet specifications.

Author: Battery Modeling
Date: 2025
"""

import numpy as np
import pandas as pd
import os as os
from typing import Tuple


class P45BDischargeGenerator:
    """
    Generates discharge curves for the Molicell INR21700-P45B cell.
    
    The generator models three discharge phases:
    1. Pre-discharge (idle, ~6s)
    2. Primary discharge (ramp from ~1A to peak current)
    3. Late discharge (plateau then decay back to zero)
    """
    
    # Cell specifications
    NOMINAL_CAPACITY = 4500  # mAh
    VOLTAGE_MAX = 4.20       # V (charge cutoff)
    VOLTAGE_MIN = 2.7      # V (discharge cutoff)
    VOLTAGE_NOMINAL = 3.6    # V (approximate 50% SOC)
    
    # OCV lookup table (SOC -> Voltage in volts)
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
        capacity_mAh: int = 4500,
        initial_soc: float = 0.80,
        ambient_temp_C: float = 23.7,
        duration_s: int = 120,
        sample_interval_ms: int = 50,
        random_seed: int = 42
    ):
        """
        Initialize the discharge curve generator.
        
        Args:
            capacity_mAh: Battery nominal capacity (default: 4500 mAh)
            initial_soc: Starting state of charge (0.0–1.0)
            ambient_temp_C: Ambient temperature in Celsius
            duration_s: Total discharge duration in seconds
            sample_interval_ms: Sample interval in milliseconds (typically 50 ms)
            random_seed: Random seed for reproducibility
        """
        self.capacity_mAh = capacity_mAh
        self.initial_soc = initial_soc
        self.ambient_temp_C = ambient_temp_C
        self.duration_s = duration_s
        self.sample_interval_ms = sample_interval_ms
        self.num_samples = int(duration_s * 1000 / sample_interval_ms)
        self.timestamps_ms = np.arange(self.num_samples) * sample_interval_ms
        self.time_s = self.timestamps_ms / 1000
        
        if random_seed is not None:
            np.random.seed(random_seed)
    
    def generate_current_profile(self) -> np.ndarray:
        """
        Generate the three-phase current profile.
        
        Phase 1 (0–5.75s): Pre-discharge at ~9 mA (idle)
        Phase 2 (5.75–73.95s): Linear ramp from ~1A to 158.5A
        Phase 3 (73.95–120s): Plateau then exponential decay to zero
        
        Returns:
            Current profile in mA
        """
        current = np.zeros(self.num_samples)
        
        # Phase 1: Pre-discharge (idle)
        phase1_end_idx = int(5.75 / 0.05)  # 115 samples
        current[0:phase1_end_idx] = 9  # ~9 mA idle current
        
        # Phase 2: Linear ramp (5.75s to 73.95s)
        phase2_start_idx = phase1_end_idx
        phase2_end_idx = int(73.95 / 0.05)  # 1479 samples
        phase2_duration = phase2_end_idx - phase2_start_idx
        
        current[phase2_start_idx:phase2_end_idx] = np.linspace(
            1000,      # Start at ~1A
            158465,    # Ramp to ~158.5A
            phase2_duration
        )
        
        # Phase 3: Plateau then decay (73.95s to 120s)
        phase3_start_idx = phase2_end_idx
        
        # Plateau for ~5 seconds
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
        
        Args:
            current_mA: Current profile in mA
            
        Returns:
            Tuple of (SOC array [0–1], OCV array in volts)
        """
        # Charge consumed per 50ms interval (in mAh)
        charge_per_step = current_mA * 0.05 / 3600  # 50ms = 50/3.6e6 hours
        cumulative_charge = np.cumsum(charge_per_step)
        
        # SOC decreases as charge is consumed
        soc = self.initial_soc - (cumulative_charge / self.capacity_mAh)
        soc = np.clip(soc, 0.0, 1.0)
        
        # Interpolate OCV from lookup table
        ocv = self._interpolate_ocv(soc)
        
        return soc, ocv
    
    def _interpolate_ocv(self, soc: np.ndarray) -> np.ndarray:
        """
        Interpolate OCV values from lookup table.
        
        Uses linear interpolation between table entries.
        """
        ocv = np.zeros(len(soc))
        soc_keys = sorted(self.OCV_TABLE.keys(), reverse=True)
        
        for i, s in enumerate(soc):
            # Find bracketing table entries
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
                # Beyond table bounds
                if s > soc_keys[0]:
                    ocv[i] = self.OCV_TABLE[soc_keys[0]]
                else:
                    ocv[i] = self.OCV_TABLE[soc_keys[-1]]
        
        return ocv
    
    def calculate_internal_resistance(self, current_mA: np.ndarray, temp_C: np.ndarray) -> np.ndarray:
        """
        Calculate internal resistance accounting for temperature and current effects.
        
        Resistance increases with:
        - Lower temperature (exponential)
        - Higher current (nonlinear)
        
        Args:
            current_mA: Current profile in mA
            temp_C: Temperature profile in Celsius
            
        Returns:
            Internal resistance in ohms
        """
        r_base = 0.050  # Base resistance at 23°C, low current (50 mΩ)
        t_coeff = -0.003  # Resistance change per °C
        t_ref = 23.0
        
        r_internal = np.zeros(len(current_mA))
        
        for i in range(len(current_mA)):
            # Temperature effect
            r_temp = r_base * (1 + t_coeff * (temp_C[i] - t_ref))
            
            # Current effect (resistance increases with current)
            i_amps = current_mA[i] / 1000
            
            if i_amps < 10:
                # Linear increase for low currents
                r_current_factor = 1.0 + 0.05 * i_amps
            else:
                # Nonlinear increase for high currents
                r_current_factor = 1.5 + 0.01 * i_amps
            
            r_internal[i] = r_temp * r_current_factor
        
        return r_internal
    
    def calculate_voltage_drop_ohmic(self, current_mA: np.ndarray, r_internal: np.ndarray) -> np.ndarray:
        """
        Calculate ohmic voltage drop: V_drop = I × R_internal
        
        Args:
            current_mA: Current in mA
            r_internal: Internal resistance in ohms
            
        Returns:
            Voltage drop in volts
        """
        i_amps = current_mA / 1000
        return i_amps * r_internal
    
    def calculate_voltage_drop_diffusion(
        self,
        current_mA: np.ndarray,
        soc: np.ndarray,
        tau_diffusion: float = 30.0
    ) -> np.ndarray:
        """
        Calculate diffusion (concentration polarization) voltage drop.
        
        This effect:
        - Increases with current
        - Builds up over time (exponential approach to steady state)
        - Worsens at low SOC (ion depletion)
        
        Args:
            current_mA: Current in mA
            soc: State of charge (0–1)
            tau_diffusion: Time constant for diffusion buildup (seconds)
            
        Returns:
            Voltage drop in volts
        """
        v_diffusion = np.zeros(len(current_mA))
        v_diff_max = 0.50  # Maximum diffusion voltage drop (500 mV)
        
        # Normalize current (10A as baseline for 4500mAh cell = 2C rate)
        i_normalized = current_mA / 10000
        i_normalized = np.clip(i_normalized, 0, 20)  # Cap at 20× baseline
        
        for i in range(len(current_mA)):
            t = self.time_s[i]
            
            # Time constant buildup
            buildup = 1 - np.exp(-t / tau_diffusion)
            
            # SOC depletion factor (effect worse at low SOC)
            soc_factor = 1 + (1 - soc[i]) * 2  # At 0% SOC, 3× effect
            
            v_diffusion[i] = v_diff_max * i_normalized[i] * buildup * soc_factor
        
        return v_diffusion
    
    def generate_temperature_profile(self, current_mA: np.ndarray, r_internal: np.ndarray) -> np.ndarray:
        """
        Generate temperature profile based on I²R heating.
        
        For the P45B, temperature rise is typically minimal (~0.3°C) except at extreme currents.
        Uses simplified model: ΔT proportional to I²R, capped at realistic values.
        
        Args:
            current_mA: Current profile in mA
            r_internal: Internal resistance in ohms
            
        Returns:
            Temperature profile in Celsius
        """
        # Use simpler model matching empirical data
        # Temperature stays relatively constant: 13.2-14.0°C
        temp = np.full(len(current_mA), self.ambient_temp_C, dtype=float)
        
        # Add small rise proportional to peak current (not cumulative heating)
        # This matches the observed ~0.8°C rise in empirical data
        max_current = np.max(current_mA) / 1000  # Convert to amps
        peak_rise = 0.8 * (np.sum(current_mA > 1000) / len(current_mA))  # Rise if high current active
        
        # Gradual rise during discharge
        for i in range(len(current_mA)):
            if current_mA[i] > 10000:  # If in high discharge region
                frac = min(i / len(current_mA), 1.0)
                temp[i] += peak_rise * frac
        
        return temp
    
    def generate_voltage_profile(
        self,
        current_mA: np.ndarray,
        soc: np.ndarray,
        ocv: np.ndarray,
        temp_C: np.ndarray,
        add_noise: bool = True,
        noise_std_mV: float = 5.0
    ) -> np.ndarray:
        """
        Generate the complete voltage profile.
        
        V(t) = OCV(SOC) - V_ohmic(I, R, T) - V_diffusion(I, SOC, t) + noise
        
        Args:
            current_mA: Current profile in mA
            soc: State of charge profile
            ocv: Open-circuit voltage profile in volts
            temp_C: Temperature profile in Celsius
            add_noise: Whether to add measurement noise
            noise_std_mV: Standard deviation of noise in mV
            
        Returns:
            Voltage profile in volts
        """
        # Calculate resistances
        r_internal = self.calculate_internal_resistance(current_mA, temp_C)
        
        # Calculate voltage drops
        v_ohmic = self.calculate_voltage_drop_ohmic(current_mA, r_internal)
        v_diffusion = self.calculate_voltage_drop_diffusion(current_mA, soc)
        
        # Synthesize voltage
        voltage = ocv - v_ohmic - v_diffusion
        
        # Clip to physical bounds
        voltage = np.clip(voltage, self.VOLTAGE_MIN, self.VOLTAGE_MAX)
        
        # Add measurement noise (optional)
        if add_noise:
            noise = np.random.normal(0, noise_std_mV / 1000, len(voltage))
            voltage = voltage + noise
        
        return voltage
    
    def generate(self) -> Tuple[pd.DataFrame, pd.DataFrame, pd.DataFrame]:
        """
        Generate complete discharge curves.
        
        Returns:
            Tuple of three DataFrames:
            - voltage_df: [timestamp_ms, voltage_mV]
            - current_df: [timestamp_ms, current_mA]
            - temperature_df: [timestamp_ms, temp_mC]
        """
        # 1. Generate current profile
        current_mA = self.generate_current_profile()
        
        # 2. Calculate SOC and OCV
        soc, ocv = self.calculate_soc_and_ocv(current_mA)
        
        # 3. Generate temperature
        r_internal_temp = self.calculate_internal_resistance(
            current_mA,
            np.full_like(current_mA, self.ambient_temp_C, dtype=float)
        )
        temp_C = self.generate_temperature_profile(current_mA, r_internal_temp)
        
        # 4. Generate voltage
        voltage_V = self.generate_voltage_profile(
            current_mA, soc, ocv, temp_C,
            add_noise=True,
            noise_std_mV=5.0
        )
        
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
    
    def save_to_csv(self, output_dir: str = '.') -> None:
        """
        Generate curves and save to CSV files.
        
        Args:
            output_dir: Directory to save CSV files
        """
        voltage_df, current_df, temperature_df = self.generate()
        
        voltage_df.to_csv(f'{output_dir}/voltage_discharge_generated.csv', header=False, index=False)
        current_df.to_csv(f'{output_dir}/current_discharge_generated.csv', header=False, index=False)
        temperature_df.to_csv(f'{output_dir}/temp_discharge_generated.csv', header=False, index=False)
        
        print(f"✓ Voltage discharge saved to {output_dir}/voltage_discharge_generated.csv")
        print(f"✓ Current discharge saved to {output_dir}/current_discharge_generated.csv")
        print(f"✓ Temperature discharge saved to {output_dir}/temp_discharge_generated.csv")


# Example usage
if __name__ == '__main__':
    # Generate curves with default parameters matching the reference data
    generator = P45BDischargeGenerator(
        capacity_mAh=4500,
        initial_soc=0.95,        # Start near full charge (~3.87V)
        ambient_temp_C=13.7,     # Match observed ambient
        duration_s=120,          # 2 minutes
        sample_interval_ms=50    # 50 ms intervals
    )
    
    # Save to CSV files
    generator.save_to_csv(f"{os.getcwd()}")
    
    # Can also get DataFrames directly
    voltage_df, current_df, temp_df = generator.generate()
    
    print("\n" + "="*70)
    print("GENERATION COMPLETE")
    print("="*70)
    print(f"\nVoltage profile:")
    print(f"  Range: {voltage_df['voltage_mV'].min()} – {voltage_df['voltage_mV'].max()} mV")
    print(f"  Mean: {voltage_df['voltage_mV'].mean():.0f} mV")
    
    print(f"\nCurrent profile:")
    print(f"  Range: {current_df['current_mA'].min()} – {current_df['current_mA'].max()} mA")
    print(f"  Mean: {current_df['current_mA'].mean():.0f} mA")
    
    print(f"\nTemperature profile:")
    print(f"  Range: {temp_df['temp_mC'].min()/1000:.2f} – {temp_df['temp_mC'].max()/1000:.2f} °C")
    print(f"  Mean: {temp_df['temp_mC'].mean()/1000:.2f} °C")
    
    # Calculate and display capacity delivered
    capacity_mAh = (current_df['current_mA'].sum() * 50 / 3600000)
    print(f"\nCapacity delivered: {capacity_mAh:.0f} mAh ({capacity_mAh/1000:.2f} Ah)")
