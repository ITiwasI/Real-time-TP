#!/bin/bash

# author: Tommaso Cucinotta <tommaso.cucinotta@santannapisa.it>

echo 0 > /sys/devices/system/cpu/intel_pstate/no_turbo
echo 100 > /sys/devices/system/cpu/intel_pstate/max_perf_pct
echo 0 > /sys/devices/system/cpu/intel_pstate/min_perf_pct

for f in no_turbo turbo_pct min_perf_pct max_perf_pct; do
    echo $f=$(cat /sys/devices/system/cpu/intel_pstate/$f)
done

echo NO_HRTICK > /sys/kernel/debug/sched_features

for c in $(ls -d /sys/devices/system/cpu/cpu[0-9]*); do
    echo powersave > $c/cpufreq/scaling_governor
    if [ -e $c/cpufreq/energy_performance_preference ]; then
	echo balance_performance > $c/cpufreq/energy_performance_preference
    fi
    if [ -f $c/power/pm_qos_resume_latency_us ]; then
	echo 0 > $c/power/pm_qos_resume_latency_us
    fi
done

echo "#CPU scaling_governor scaling_cur_freq energy_perf_pref resume_latency"
for c in $(ls -d /sys/devices/system/cpu/cpu[0-9]*); do
    echo $c $(cat $c/cpufreq/scaling_governor) $(cat $c/cpufreq/scaling_cur_freq) $(cat $c/cpufreq/energy_performance_preference 2> /dev/null) $(cat $c/power/pm_qos_resume_latency_us 2> /dev/null)
done
