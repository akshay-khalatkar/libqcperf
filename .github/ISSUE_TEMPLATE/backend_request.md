---
name: Backend request
about: Request a new backend or backend enhancement
title: '[BACKEND] '
labels: backend, enhancement
assignees: ''

---

## Backend Type
- [ ] New Backend
- [ ] Enhancement to Existing Backend

## Backend Name
[e.g., GPU Performance, Network Bandwidth, Storage I/O]

## Description
A clear and concise description of the backend and what metrics it would provide.

## Metrics to Monitor
List the specific metrics this backend should track:
1. Metric 1: [e.g., GPU Temperature] - Unit: [e.g., °C]
2. Metric 2: [e.g., GPU Frequency] - Unit: [e.g., MHz]
3. Metric 3: [e.g., GPU Utilization] - Unit: [e.g., %]

## Data Source
Describe how this backend would collect data:
- Driver/API: [e.g., Windows ETW, Linux sysfs, IOCTL]
- Access Method: [e.g., Kernel driver, System calls, Hardware registers]
- Permissions Required: [e.g., Administrator, Root, Standard user]

## Target Environment
- OS: [e.g., Windows 11, Linux]
- Arch: [e.g., ARM, ARM64, X64]
- Hardware: [e.g., Snapdragon 8cx Gen 3, Snapdragon X Elite]
- Driver Version: [if applicable]

## Use Case
Describe why this backend is needed:
- What problem does it solve?
- Who would benefit from this backend?
- What applications or scenarios would use it?

## Sampling/Streaming Rates
Suggested rates for data collection:
- Sampling Rates: [e.g., 50ms, 100ms, 200ms]
- Streaming Rates: [e.g., 200ms, 500ms, 1000ms]

## Implementation Notes
If you have implementation suggestions:
- Are there existing libraries or APIs that could be used?
- Are there similar implementations in other projects?
- What are the technical challenges?
- Performance considerations?

## Example Output
If possible, provide an example of what the data output should look like:
```
Capability: gpu-performance
Metrics:
  - GPU Temperature: 45.5°C
  - GPU Frequency: 850MHz
  - GPU Utilization: 75%
```

## Additional Context
Add any other context, documentation links, or examples about the backend request here.

## Checklist
- [ ] I have searched existing issues to ensure this is not a duplicate
- [ ] I have clearly described the metrics to be monitored
- [ ] I have identified the data source and access method
- [ ] I have described the use case for this backend
- [ ] I have considered platform compatibility
