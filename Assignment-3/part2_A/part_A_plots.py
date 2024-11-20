import xml.etree.ElementTree as ET
import matplotlib.pyplot as plt

# Load and parse XML file
tree = ET.parse('A_flow-monitor.xml')
root = tree.getroot()

# Define namespaces for XML parsing
ns = {'ns': 'http://www.w3.org/2001/XMLSchema-instance'}

# Initialize data storage
flows = []
throughput = []
packet_loss = []

# Extract FlowStats and FlowClassifier
flow_stats = root.find('FlowStats')
flow_classifier = root.find('Ipv4FlowClassifier')

# Create dictionaries to map flowId to flow information
flow_id_to_info = {}

for flow in flow_classifier.findall('Flow'):
    flow_id = flow.get('flowId')
    src = flow.get('sourceAddress')
    dst = flow.get('destinationAddress')
    srcPort = flow.get('sourcePort')
    dstPort = flow.get('destinationPort')
    flow_id_to_info[flow_id] = {'src': f"{src}:{srcPort}", 'dst': f"{dst}:{dstPort}"}

# Extract FlowStats data
for flow in flow_stats.findall('Flow'):
    flow_id = flow.get('flowId')
    lost_packets = int(flow.get('lostPackets'))
    rx_bytes = int(flow.get('rxBytes'))
    rx_packets = int(flow.get('rxPackets'))
    
    
    # Calculate throughput in Mbps
    time_period_ns = float(flow.get('timeLastRxPacket').replace('+', '').replace('ns', '')) - float(flow.get('timeFirstTxPacket').replace('+', '').replace('ns', ''))
    time_period_s = time_period_ns / 1e9
    throughput_mbps = (rx_bytes * 8) / (time_period_s * 1e6)  # Convert bytes to bits and time to seconds
    
    flow_info = flow_id_to_info.get(flow_id, {})
    flow_label = f"{flow_info.get('src')} -> {flow_info.get('dst')}"
    
    flows.append(flow_label)
    throughput.append(throughput_mbps)
    packet_loss.append(lost_packets)

# Plot throughput vs flow
plt.figure(figsize=(12, 6))
plt.bar(flows, throughput, color='blue')
plt.xlabel('Flow')
plt.ylabel('Throughput (Mbps)')
plt.title('Flow vs Throughput')
plt.xticks(rotation=30)
plt.tight_layout()
plt.show()

# Plot packet loss vs flow
plt.figure(figsize=(12, 6))
plt.bar(flows, packet_loss, color='red')
plt.xlabel('Flow')
plt.ylabel('Total Packets Lost')
plt.title('Flow vs Total Packets Lost')
plt.xticks(rotation=30)
plt.tight_layout()
plt.show()
