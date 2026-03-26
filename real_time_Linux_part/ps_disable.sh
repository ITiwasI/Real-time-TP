#!/bin/bash

# author: Tommaso Cucinotta <tommaso.cucinotta@santannapisa.it>

if [ -e /sys/devices/system/cpu/intel_pstate/no_turbo ]; then
    echo 1 > /sys/devices/system/cpu/intel_pstate/no_turbo
fi

echo HRTICK > /sys/kernel/debug/sched_features

turbo_pct=$(cat /sys/devices/system/cpu/intel_pstate/turbo_pct)
perf_pct=$[ 100 - $turbo_pct ]
echo $perf_pct > /sys/devices/system/cpu/intel_pstate/max_perf_pct
echo $perf_pct > /sys/devices/system/cpu/intel_pstate/min_perf_pct

for f in no_turbo turbo_pct min_perf_pct max_perf_pct; do
    echo $f=$(cat /sys/devices/system/cpu/intel_pstate/$f)
done

for c in $(ls -d /sys/devices/system/cpu/cpu[0-9]*); do
    echo performance > $c/cpufreq/scaling_governor
    if [ -e $c/cpufreq/energy_performance_preference ]; then
	echo performance > $c/cpufreq/energy_performance_preference
    fi
    if [ -f $c/power/pm_qos_resume_latency_us ]; then
	echo 1 > $c/power/pm_qos_resume_latency_us
    fi
done

echo "#CPU scaling_governor scaling_cur_freq energy_perf_pref resume_latency"
for c in $(ls -d /sys/devices/system/cpu/cpu[0-9]*); do
    echo $c $(cat $c/cpufreq/scaling_governor) $(cat $c/cpufreq/scaling_cur_freq) $(cat $c/cpufreq/energy_performance_preference 2> /dev/null) $(cat $c/power/pm_qos_resume_latency_us 2> /dev/null)
done
