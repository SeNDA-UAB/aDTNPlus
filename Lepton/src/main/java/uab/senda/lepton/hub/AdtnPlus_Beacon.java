/*
 * Copyright (c) 2018 SeNDA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
package uab.senda.lepton.hub;

import java.net.DatagramPacket;
import java.net.InetSocketAddress;
import java.util.ArrayList;

import casa.lepton.hub.Beacon;

/**
 * FILE AdtnPlus_Beacon.java
 * AUTHOR Blackcatn13
 * DATE Jun 13, 2018
 * VERSION 1
 * This file contains the aDTN Beacon adapter for LEPTON.
 */
public class AdtnPlus_Beacon implements Beacon {

	private String source_ = null;
	private int port_ = 0;
	private String ip_ = null;
	
	private ArrayList<String> eids_ = new ArrayList<String>(); 
	
	
	public AdtnPlus_Beacon(DatagramPacket packet) throws Exception {
		decode(packet.getData(), packet.getOffset(), packet.getLength());
	}
	
	public String toString() {
		String res = "";
		res += '\t' + "Source: " + source_ + "\tPort: " + port_ + "\tIP: " + ip_;
		return res;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see casa.lepton.hub.Beacon#decode(byte[], int, int)
	 */
	@Override
	public void decode(byte[] buffer, int offset, int length) throws Exception {
		String s = new String(buffer);
		int idx0 = offset;
		int idx1 = s.indexOf('\0', idx0);
		source_ = s.substring(idx0, idx1);
		idx0 = idx1 + 1;
		idx1 = s.indexOf('\0', idx0);
		ip_ = s.substring(idx0, idx1);
		idx0 = idx1 + 1;
		idx1 = s.indexOf('\0', idx0);
		port_ = Integer.parseInt(s.substring(idx0, idx1));
		idx0 = idx1 + 1;
		idx1 = s.indexOf('\0', idx0);
		int eid_size = Integer.parseInt(s.substring(idx0, idx1));
		for (int i = 0; i < eid_size; i++) {
			idx0 = idx1 + 1;
			idx1 = s.indexOf('\0', idx0);
			String eid = s.substring(idx0, idx1);
			eids_.add(eid);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see casa.lepton.hub.Beacon#encode(byte[], int)
	 */
	@Override
	public int encode(byte[] buffer, int offset) throws Exception {
		String s = "";
		s += source_ + '\0' + ip_ + '\0' + port_ + '\0' + eids_.size() + '\0';
		for (int i = 0; i < eids_.size(); i++) {
			s += eids_.get(i) + '\0';
		}
		java.lang.System.arraycopy(s.getBytes(), 0, buffer, offset, s.getBytes().length);
		//buffer = s.getBytes();
		return offset + buffer.length;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see casa.lepton.hub.Beacon#getBeaconType()
	 */
	@Override
	public BeaconType getBeaconType() {
		// aDTN beacons are all "HELLO" beacons: there is no way for a
		// host to announce it is leaving the network by broadcasting
		// a "BYE" beacon
		return BeaconType.HELLO;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see casa.lepton.hub.Beacon#getDestination()
	 */
	@Override
	public String getDestination() {
		// aDTN beacons are always broadcast, so no specific
		// destination is ever specified
		return null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see casa.lepton.hub.Beacon#getGossipingAddress(casa.lepton.hub.Beacon.
	 * GossipingType)
	 */
	@Override
	public InetSocketAddress getGossipingAddress(GossipingType type) {
		InetSocketAddress res = null;
		if (type == GossipingType.TCP) {
			res = new InetSocketAddress(ip_, port_);
		}
		
		return res;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see casa.lepton.hub.Beacon#getSource()
	 */
	@Override
	public String getSource() {
		return source_;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see casa.lepton.hub.Beacon#hasGossipingType(casa.lepton.hub.Beacon.
	 * GossipingType)
	 */
	@Override
	public boolean hasGossipingType(GossipingType type) {
		switch (type) {
		case TCP:
			return true;
		default:
			return false;
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see casa.lepton.hub.Beacon#setGossipingAddress(casa.lepton.hub.Beacon.
	 * GossipingType, java.net.InetSocketAddress)
	 */
	@Override
	public void setGossipingAddress(GossipingType type, InetSocketAddress addr) throws Exception {
		if (type == GossipingType.TCP) {
			//ip_ = addr.getAddress().getHostAddress();
			port_ = addr.getPort();
		}

	}

}
