% pid_simulation.m
% Differential-drive line-following car PID simulation
% Models the MSPM0G3507 car with 3-sensor grayscale + PID control
%
% Reads parameters from sys_config.h and PID logic from pid.c
% Does NOT modify any existing files.
%
% Simulation of car dynamics:
%   - 3 grayscale sensors (left, center, right) at ~1.5cm spacing
%   - Differential drive steering via PID output
%   - Line is black tape (~2cm wide) on white surface
%   - Car kinematics: dx/dt = v*cos(theta), dy/dt = v*sin(theta), dtheta/dt = omega

clear; clc; close all;

%% ===== Physical Parameters (from actual car) =====
WHEEL_BASE   = 0.12;      % Wheel spacing (m) - ~12cm
WHEEL_RADIUS = 0.032;     % Wheel radius (m) - ~3.2cm
SENSOR_SPACING = 0.015;   % Distance between adjacent sensors (m) - ~1.5cm
LINE_WIDTH   = 0.020;     % Black tape width (m) - 2cm
BASE_SPEED   = 0.3;       % Nominal forward speed (m/s)
MAX_DIFF     = 0.15;      % Max speed difference from PID (m/s)

% Sensor sensing range: sensor returns ~0 (white) to ~1 (black)
% Sensor value = f(distance from sensor center to line center)
SENSOR_RANGE = 0.008;     % Half-width of sensor field of view (m)

% ADC simulation: maps sensor [0,1] to ADC [0, 4095]
ADC_MAX = 4095;
THRESHOLD = 1000;  % GRAY_STOP_THRESHOLD from sys_config.h

%% ===== Simulation Parameters =====
DT = 0.001;           % Integration timestep (s) - 1ms
SIM_TIME = 10.0;      % Total simulation time (s)
t = 0:DT:SIM_TIME;
N = length(t);

% PID update rate (matches PID_PERIOD_MS = 10 from sys_config.h)
PID_DT = 0.010;
pid_steps_per_loop = round(PID_DT / DT);

%% ===== Track Definition =====
% Define a track as a series of waypoints for the line center
% Track types to test:
%   1. Straight line with initial offset
%   2. Gentle curve (sinusoidal)
%   3. Sharp S-curve

% We'll use a parametric curve for the line center
% Line center position: (x_line(s), y_line(s)) where s = forward distance

% For simulation simplicity, model the line as y = f(x)
% Track: straight + sine wave

% Track function: line center position y as function of x
% Straight with sine perturbation
track_type = 2;  % 1=straight, 2=sine, 3=s-curve

%% ===== PID Parameter Sets to Test =====
% Separate arrays for clarity and MATLAB compatibility
pid_Kp = [0.5, 0.5, 1.0, 1.0, 1.0, ...
          1.5, ...
          1.5, 1.5, 2.0, 2.0, 2.0, ...
          2.5, 2.5, 3.0, 3.0, ...
          1.5, 2.0];

pid_Ki = [0.0, 0.05, 0.0, 0.05, 0.1, ...
          0.1, ...
          0.05, 0.2, 0.05, 0.1, 0.2, ...
          0.05, 0.1, 0.05, 0.1, ...
          0.5, 0.3];

pid_Kd = [0.0, 0.0, 0.0, 0.1, 0.2, ...
          0.5, ...
          0.3, 0.8, 0.5, 1.0, 0.5, ...
          1.5, 1.0, 1.5, 2.0, ...
          0.5, 1.0];

pid_labels = {
    'P-only (Kp=0.5)',
    'PI (Kp=0.5,Ki=0.05)',
    'P-only (Kp=1.0)',
    'PID (Kp=1.0,Ki=0.05,Kd=0.1)',
    'PID (Kp=1.0,Ki=0.1,Kd=0.2)',
    'CURRENT (Kp=1.5,Ki=0.1,Kd=0.5)',
    'PID (Kp=1.5,Ki=0.05,Kd=0.3)',
    'PID (Kp=1.5,Ki=0.2,Kd=0.8)',
    'PID (Kp=2.0,Ki=0.05,Kd=0.5)',
    'PID (Kp=2.0,Ki=0.1,Kd=1.0)',
    'PID (Kp=2.0,Ki=0.2,Kd=0.5)',
    'PID (Kp=2.5,Ki=0.05,Kd=1.5)',
    'PID (Kp=2.5,Ki=0.1,Kd=1.0)',
    'PID (Kp=3.0,Ki=0.05,Kd=1.5)',
    'PID (Kp=3.0,Ki=0.1,Kd=2.0)',
    'PID (Kp=1.5,Ki=0.5,Kd=0.5)',
    'PID (Kp=2.0,Ki=0.3,Kd=1.0)',
};

num_sets = length(pid_Kp);

%% ===== Storage for Results =====
results = struct();

%% ===== Run Simulation for Each PID Set =====
fprintf('==========================================================\n');
fprintf('  PID Parameter Sweep - Differential Line-Following Car\n');
fprintf('==========================================================\n');
fprintf('Track type: %d  |  Sim time: %.1fs  |  PID period: %dms\n', ...
    track_type, SIM_TIME, round(PID_DT*1000));
fprintf('Base speed: %.2f m/s  |  Wheel base: %.1fcm\n', ...
    BASE_SPEED, WHEEL_BASE*100);
fprintf('==========================================================\n\n');

for set_idx = 1:num_sets
    Kp = pid_Kp(set_idx);
    Ki = pid_Ki(set_idx);
    Kd = pid_Kd(set_idx);
    label = pid_labels{set_idx};

    % Run simulation
    [error_hist, pos_hist, pid_out_hist, max_dev, settle_time, ...
     rms_error, overshoot, osc_count] = ...
        simulate_car(Kp, Ki, Kd, t, DT, PID_DT, pid_steps_per_loop, ...
                     WHEEL_BASE, BASE_SPEED, MAX_DIFF, ...
                     SENSOR_SPACING, SENSOR_RANGE, LINE_WIDTH, ...
                     ADC_MAX, THRESHOLD, track_type);

    % Store results
    results(set_idx).Kp = Kp;
    results(set_idx).Ki = Ki;
    results(set_idx).Kd = Kd;
    results(set_idx).label = label;
    results(set_idx).max_deviation = max_dev;
    results(set_idx).settle_time = settle_time;
    results(set_idx).rms_error = rms_error;
    results(set_idx).overshoot = overshoot;
    results(set_idx).osc_count = osc_count;
    results(set_idx).error_hist = error_hist;
    results(set_idx).pos_hist = pos_hist;
    results(set_idx).pid_out_hist = pid_out_hist;
end

%% ===== Scoring and Ranking =====
% Normalize each metric to [0,1] (0=best, 1=worst) and compute weighted score

max_devs = [results.max_deviation];
settle_ts = [results.settle_time];
rms_errs = [results.rms_error];
over_shoots = [results.overshoot];
osc_counts = [results.osc_count];

% Normalize (handle edge cases)
norm_max_dev  = (max_devs - min(max_devs)) / (max(max_devs) - min(max_devs) + 1e-6);
norm_settle   = (settle_ts - min(settle_ts)) / (max(settle_ts) - min(settle_ts) + 1e-6);
norm_rms      = (rms_errs - min(rms_errs)) / (max(rms_errs) - min(rms_errs) + 1e-6);
norm_over     = (over_shoots - min(over_shoots)) / (max(over_shoots) - min(over_shoots) + 1e-6);
norm_osc      = (osc_counts - min(osc_counts)) / (max(osc_counts) - min(osc_counts) + 1e-6);

% Weights: max_dev=0.25, settle_time=0.20, rms_error=0.30, overshoot=0.15, oscillation=0.10
weights = [0.25, 0.20, 0.30, 0.15, 0.10];

for i = 1:num_sets
    results(i).score = weights(1)*norm_max_dev(i) + weights(2)*norm_settle(i) ...
                     + weights(3)*norm_rms(i) + weights(4)*norm_over(i) ...
                     + weights(5)*norm_osc(i);
end

% Sort by score (lower = better)
[~, sort_idx] = sort([results.score]);
results = results(sort_idx);

%% ===== Display Results =====
fprintf('\n==========================================================\n');
fprintf('  RESULTS - RANKED BY OVERALL PERFORMANCE\n');
fprintf('==========================================================\n');
fprintf('%-5s %-6s %-6s %-6s %-10s %-11s %-10s %-10s %-8s %-6s\n', ...
    'Rank', 'Kp', 'Ki', 'Kd', 'MaxDev(cm)', 'Settle(ms)', ...
    'RMS(mm)', 'Over(%)', 'Osc(#)', 'Score');
fprintf('%-5s %-6s %-6s %-6s %-10s %-11s %-10s %-10s %-8s %-6s\n', ...
    '----', '------', '------', '------', '----------', '-----------', ...
    '--------', '--------', '------', '-----');

for i = 1:num_sets
    r = results(i);
    fprintf('%-5d %-6.2f %-6.2f %-6.2f %-10.2f %-11.0f %-10.2f %-10.1f %-8d %-6.3f\n', ...
        i, r.Kp, r.Ki, r.Kd, ...
        r.max_deviation*100, r.settle_time*1000, r.rms_error*1000, ...
        r.overshoot*100, r.osc_count, r.score);
end

%% ===== Recommendation =====
fprintf('\n==========================================================\n');
fprintf('  RECOMMENDATION\n');
fprintf('==========================================================\n');

best = results(1);
fprintf('\n*** BEST OVERALL: %s ***\n', best.label);
fprintf('    Score: %.3f\n', best.score);
fprintf('    Max deviation: %.2f cm\n', best.max_deviation*100);
fprintf('    Settle time: %.0f ms\n', best.settle_time*1000);
fprintf('    RMS error: %.2f mm\n', best.rms_error*1000);

fprintf('\n--- Top 3 Recommended PID Sets ---\n');
for i = 1:min(3, num_sets)
    r = results(i);
    fprintf('  #%d: Kp=%.2f  Ki=%.2f  Kd=%.2f  (score=%.3f)  [%s]\n', ...
        i, r.Kp, r.Ki, r.Kd, r.score, r.label);
end

% Current defaults
for i = 1:num_sets
    if results(i).Kp == 1.5 && results(i).Ki == 0.1 && results(i).Kd == 0.5
        current_rank = i;
        fprintf('\n--- Current sys_config.h defaults (Kp=1.5, Ki=0.1, Kd=0.5) ---\n');
        fprintf('  Rank: #%d  Score: %.3f\n', current_rank, results(i).score);
        break;
    end
end

%% ===== Plotting =====
% Figure 1: Error over time for top 5 PID sets
figure('Name', 'PID Comparison - Line Error', 'Position', [100, 100, 1200, 800]);

subplot(3,1,1);
hold on;
colors = lines(min(5, num_sets));
leg_entries = {};
for i = 1:min(5, num_sets)
    r = results(i);
    plot(t, r.error_hist*100, 'Color', colors(i,:), 'LineWidth', 1.5);
    leg_entries{end+1} = sprintf('#%d: %s', i, r.label);
end
yline(0, 'k--');
xlabel('Time (s)');
ylabel('Lateral Error (cm)');
title('Line-Following Error Comparison (Top 5 PID Sets)');
legend(leg_entries, 'Location', 'best');
grid on;

% Figure 1b: Car trajectory for top 3
subplot(3,1,2);
hold on;
% Plot reference line
x_ref = 0:0.01:3;
y_ref = get_line_position(x_ref, track_type);
plot(x_ref, y_ref*100, 'k-', 'LineWidth', 2, 'DisplayName', 'Line center');

for i = 1:min(3, num_sets)
    r = results(i);
    px = r.pos_hist(1,:);
    py = r.pos_hist(2,:);
    plot(px*100, py*100, 'Color', colors(i,:), 'LineWidth', 1.5, ...
        'DisplayName', sprintf('#%d: %s', i, r.label));
end
xlabel('X (cm)');
ylabel('Y (cm)');
title('Car Trajectory vs Line Center (Top 3)');
legend('Location', 'best');
grid on;
axis equal;

% Figure 1c: PID output for top 5
subplot(3,1,3);
hold on;
for i = 1:min(5, num_sets)
    r = results(i);
    plot(t, r.pid_out_hist, 'Color', colors(i,:), 'LineWidth', 1.5);
end
yline(0, 'k--');
xlabel('Time (s)');
ylabel('PID Output');
title('PID Controller Output Comparison');
legend(leg_entries, 'Location', 'best');
grid on;

% Figure 2: Score bar chart
figure('Name', 'PID Score Comparison', 'Position', [150, 150, 1400, 600]);
subplot(1,2,1);
score_values = [results.score];
bar(score_values);
set(gca, 'XTickLabel', {results.label}, 'XTick', 1:num_sets);
xtickangle(45);
ylabel('Score (lower = better)');
title('Overall Performance Score');
grid on;

subplot(1,2,2);
% Radar-like: show top 3 metrics
top_n = min(5, num_sets);
metric_names = {'MaxDev', 'Settle', 'RMS', 'Overshoot', 'Osc'};
all_metrics = [norm_max_dev(sort_idx); norm_settle(sort_idx); ...
               norm_rms(sort_idx); norm_over(sort_idx); norm_osc(sort_idx)]';
bar(all_metrics(1:top_n, :)');
set(gca, 'XTickLabel', metric_names);
legend({results(1:top_n).label}, 'Location', 'best');
ylabel('Normalized Score (lower = better)');
title('Metric Breakdown for Top 5');
grid on;

fprintf('\nSimulation complete. See figures for details.\n');

%% ===== Helper Functions =====

function [error_hist, pos_hist, pid_out_hist, max_dev, settle_time, ...
          rms_error, overshoot, osc_count] = ...
    simulate_car(Kp, Ki, Kd, t, DT, PID_DT, pid_steps_per_loop, ...
                 wheel_base, base_speed, max_diff, ...
                 sensor_spacing, sensor_range, line_width, ...
                 adc_max, threshold, track_type)

    N = length(t);

    % Car state: [x, y, theta]
    % x, y = position of car center (sensor mount point)
    % theta = heading angle (0 = along +x)
    state = [0; 0; 0];  % Start at origin, heading +x

    % Initial lateral offset for step response test
    state(2) = 0.03;  % Start 3cm off the line

    % PID state (matching pid.c implementation)
    integral = 0;
    prev_error = 0;
    INT_LIMIT = 50;    % From PID_INT_LIMIT in sys_config.h
    OUT_LIMIT = 100;   % From PID_OUT_LIMIT in sys_config.h

    % History
    error_hist = zeros(1, N);
    pos_hist = zeros(2, N);
    pid_out_hist = zeros(1, N);

    % Metrics
    max_dev = 0;
    settle_threshold = 0.002;  % 2mm
    settle_time = t(end);
    settled = false;
    overshoot = 0;
    osc_count = 0;
    prev_error_sign = 0;
    pid_counter = 0;

    for i = 1:N
        x = state(1);
        y = state(2);
        theta = state(3);

        % --- Simulate 3 grayscale sensors ---
        % Sensor positions in car frame:
        % Left sensor:  at (0, +sensor_spacing) relative to center
        % Center sensor: at (0, 0)
        % Right sensor:  at (0, -sensor_spacing)

        % Sensor global positions
        cos_t = cos(theta);
        sin_t = sin(theta);

        sx_left   = x + sensor_spacing * (-sin_t);
        sy_left   = y + sensor_spacing * cos_t;
        sx_center = x;
        sy_center = y;
        sx_right  = x - sensor_spacing * (-sin_t);
        sy_right  = y - sensor_spacing * cos_t;

        % Line position at each sensor's x-coordinate
        line_y_left   = get_line_position(sx_left, track_type);
        line_y_center = get_line_position(sx_center, track_type);
        line_y_right  = get_line_position(sx_right, track_type);

        % Distance from sensor to line center (lateral offset)
        dist_left   = sy_left - line_y_left;
        dist_center = sy_center - line_y_center;
        dist_right  = sy_right - line_y_right;

        % Sensor reading model: Gaussian-like response
        % Sensor returns high when over black line, low when over white
        % ADC = (1 - exp(-dist^2/(2*sigma^2))) mapped to [0, ADC_MAX]
        sigma_sensor = sensor_range / 2;
        adc_left   = adc_max * (1 - exp(-dist_left^2 / (2*sigma_sensor^2)));
        adc_center = adc_max * (1 - exp(-dist_center^2 / (2*sigma_sensor^2)));
        adc_right  = adc_max * (1 - exp(-dist_right^2 / (2*sigma_sensor^2)));

        % --- PID Calculation (matches grayscale_get_error + pid_calculate) ---
        % Every PID_DT seconds
        if mod(i-1, pid_steps_per_loop) == 0
            pid_counter = pid_counter + 1;

            % grayscale_get_error: weighted average
            left_on   = max(0, adc_left - threshold);
            center_on = max(0, adc_center - threshold);
            right_on  = max(0, adc_right - threshold);
            total_on = left_on + center_on + right_on;

            if total_on > 0
                error = (left_on * (-2) + right_on * 2) / total_on;
                % Dead zone
                if abs(error) < 0.5
                    error = 0;
                end
            end

            % pid_calculate
            p_term = Kp * error;
            integral = integral + Ki * error;
            % Anti-windup
            if integral > INT_LIMIT
                integral = INT_LIMIT;
            elseif integral < -INT_LIMIT
                integral = -INT_LIMIT;
            end
            i_term = integral;
            d_term = Kd * (error - prev_error);
            prev_error = error;
            pid_output = p_term + i_term + d_term;
            % Output clamping
            if pid_output > OUT_LIMIT
                pid_output = OUT_LIMIT;
            elseif pid_output < -OUT_LIMIT
                pid_output = -OUT_LIMIT;
            end
        end

        % --- Car Kinematics ---
        % PID output maps to differential speed
        % pid_output in [-100, 100] maps to speed diff [-max_diff, +max_diff]
        diff_speed = (pid_output / OUT_LIMIT) * max_diff;

        v_left  = base_speed - diff_speed;
        v_right = base_speed + diff_speed;

        % Clamp wheel speeds
        v_left  = max(0, min(v_left, 2*base_speed));
        v_right = max(0, min(v_right, 2*base_speed));

        % Differential drive kinematics
        v = (v_right + v_left) / 2;
        omega = (v_right - v_left) / wheel_base;

        % Update state (Euler integration)
        state(1) = state(1) + v * cos_t * DT;
        state(2) = state(2) + v * sin_t * DT;
        state(3) = state(3) + omega * DT;

        % --- Record ---
        error_hist(i) = error;
        pos_hist(:, i) = [x; y];
        pid_out_hist(i) = pid_output;

        % --- Metrics accumulation ---
        abs_dev = abs(error);  % error is in "sensor units", approximate: 1 unit ≈ sensor_spacing

        % Convert error to approximate lateral distance
        % When one side sensor is fully on line and other is off:
        % error ≈ 2 → lateral offset ≈ sensor_spacing
        % So: lateral_offset ≈ error * sensor_spacing / 2
        lateral_dev = abs(error) * sensor_spacing / 2;
        max_dev = max(max_dev, lateral_dev);

        % Settling time
        if ~settled && i > round(0.5/DT)  % Skip first 0.5s
            if lateral_dev < settle_threshold
                % Count consecutive samples below threshold
                settled_early = true;
                for jj = max(1, i-round(0.2/DT)):i
                    if abs(error_hist(jj)) * sensor_spacing / 2 >= settle_threshold
                        settled_early = false;
                        break;
                    end
                end
                if settled_early
                    settle_time = t(i);
                    settled = true;
                end
            end
        end

        % Overshoot tracking
        overshoot = max(overshoot, lateral_dev);

        % Oscillation counting (zero crossings of error)
        if i > 1
            curr_sign = sign(error);
            if prev_error_sign ~= 0 && curr_sign ~= 0 && curr_sign ~= prev_error_sign
                osc_count = osc_count + 1;
            end
            if curr_sign ~= 0
                prev_error_sign = curr_sign;
            end
        end
    end

    % RMS error
    lateral_errors = abs(error_hist) * sensor_spacing / 2;
    rms_error = sqrt(mean(lateral_errors(round(0.5/DT):end).^2));

    % If never settled, use max time
    if ~settled
        settle_time = t(end);
    end

    % Overshoot ratio (relative to steady-state; approximate)
    if max_dev > settle_threshold
        overshoot = (overshoot - lateral_errors(end)) / (max_dev + 1e-6);
    else
        overshoot = 0;
    end
    overshoot = max(0, overshoot);
end

function y = get_line_position(x, track_type)
    % Returns the y-coordinate of the line center at position x
    switch track_type
        case 1
            % Pure straight line
            y = 0 * x;
        case 2
            % Sine wave: amplitude 5cm, wavelength 1m
            y = 0.05 * sin(2 * pi * x / 1.0);
        case 3
            % S-curve: sharp transitions
            y = 0.08 * sin(2 * pi * x / 0.5);
    end
end
