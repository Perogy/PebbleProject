<?php

/** -

Donations welcome:
	BTC: 14HfjbCERqaK61YZogkKzSPw7GwP4r86TB
	LTC: LKvJ6UpMtmS5tcsQzmgegzm7vjxAziFgZe
		~ Thank you!

MIT License (MIT)

Copyright (c) 2013 http://coinwidget.com/ 
Copyright (c) 2013 http://scotty.cc/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

	header("Content-type: text/javascript");
	/*
		you should server side cache this response, especially if your site is active
	*/
	$data = isset($_GET['data'])?$_GET['data']:'';
	if (!empty($data)) {
		$data = explode("|", $data);
		$responses = array();
		if (!empty($data)) {
			foreach ($data as $key) {
				list($instance,$currency,$address) = explode('_',$key);
				switch ($currency) {
					case 'bitcoin': 
						$response = get_bitcoin($address);
						break;
					case 'litecoin': 
						$response = get_litecoin($address);
						break;
					case 'dogecoin': 
						$response = get_dogecoin($address);
						break;
					case 'peercoin': 
						$response = get_peercoin($address);
						break;
					case 'quark': 
						$response = get_quark($address);
						break;
					case 'primecoin': 
						$response = get_primecoin($address);
						break;
					case 'monacoin': 
						$response = get_monacoin($address);
						break;
					case 'sakuracoin': 
						$response = get_sakuracoin($address);
						break;
					case 'sha1coin': 
						$response = get_sha1coin($address);
						break;	
				}
				$responses[$instance] = $response;
			}
		}
		echo 'var COINWIDGETCOM_DATA = '.json_encode($responses).';';
	}

	function get_bitcoin($address) {
		$return = array();
		$data = get_request('http://blockexplorer.com/address/'.$address);
		if (!empty($data) 
		  && strstr($data, 'Received transactions: ') 
		  && strstr($data, 'Received BTC: ')) {
		  	$return += array(
				'count' => (int) parse($data,'Received transactions: ','</li>'),
				'amount' => (float) parse($data,'Received BTC: ','</li>')
			);
		  	return $return;
		}
	}

	function get_litecoin($address) {
		$return = array();
		$data = get_request('http://explorer.litecoin.net/address/'.$address);
		if (!empty($data) 
		  && strstr($data, 'Transactions in: ') 
		  && strstr($data, 'Received: ')) {
		  	$return += array(
				'count' => (int) parse($data,'Transactions in: ','<br />'),
				'amount' => (float) parse($data,'Received: ','<br />')
			);
		  	return $return;
		}
	}
	
	function get_dogecoin($address) {
			$return = array();
			$data = get_request('http://dogechain.info/address/'.$address);
			if (!empty($data)) {
				$return += array(
				'count' => (int) strip_tags(parse($data,'Transactions in','</tr>')),
				'amount' => (float) strip_tags(parse($data,'Received','</tr>'))
			);
			return $return;
			}
		}
	
	function get_peercoin($address) {
		$return = array();
		$data = get_request('http://ppc.cryptocoinexplorer.com/address/'.$address);
		if (!empty($data)) {
		  	$return += array(
				'count' => (int) parse($data,'Transactions in: ','<br />'),
				'amount' => (float) parse($data,'Received: ','<br />')
			);
		  	return $return;
		}
	}
 
 	function get_quark($address) {
 		/* it also counts transactions out */
	$return = array();
	$data = get_request('http://qrk.blockr.io/api/v1/address/info/' . $address);
	if (!empty($data)) {
	$data = json_decode($data);
		$return += array(
		'count' => (int) $data->data->nb_txs,
		'amount' => (float) $data->data->totalreceived
		);
	return $return;
	}
	}
	
	function get_primecoin($address) {
		/* it also counts transactions out */
		$return = array();
		$data = get_request('https://coinplorer.com/XPM/Addresses/'.$address);
		if (!empty($data)) {
		  	$return += array(
				'count' => (int) strip_tags(parse($data,'Transactions count:','</tr>')),
				'amount' => (float) strip_tags(parse($data,'Total received:','</tr>'))
			);
		  	return $return;
		}
	}
	
	
	
	function get_monacoin($address) {
		$return = array();
		$data = get_request('http://abe.cryptocoinservice.net/address/'.$address);
		if (!empty($data)) {
		  	$return += array(
				'count' => (int) parse($data,'Transactions in: ','<br />'),
				'amount' => (float) parse($data,'Received: ','<br />')
			);
		  	return $return;
		}
	}
	
	function get_sakuracoin($address) {
		$return = array();
		$data = get_request('http://abe.sighash.info/address/'.$address);
		if (!empty($data)) {
		  	$return += array(
				'count' => (int) parse($data,'Transactions in: ','<br />'),
				'amount' => (float) parse($data,'Received: ','<br />')
			);
		  	return $return;
		}
	}

	
	function get_sha1coin($address) {
		$return = array();
		$data = get_request('http://abe.sighash.info/address/'.$address);
		if (!empty($data)) {
		  	$return += array(
				'count' => (int) parse($data,'Transactions in: ','<br />'),
				'amount' => (float) parse($data,'Received: ','<br />')
			);
		  	return $return;
		}
	}	

	function get_request($url,$timeout=4) {
		if (function_exists('curl_version')) {
			$curl = curl_init();
			curl_setopt($curl, CURLOPT_URL, $url);
			curl_setopt($curl, CURLOPT_RETURNTRANSFER,true);
			curl_setopt($curl, CURLOPT_CONNECTTIMEOUT, $timeout);
			curl_setopt($curl, CURLOPT_FOLLOWLOCATION, true);
			curl_setopt($curl, CURLOPT_USERAGENT,'Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.13');
			$return = curl_exec($curl);
			curl_close($curl);
			return $return;
		} else {
			return @file_get_contents($url);
		}
	}

	function parse($string,$start,$stop) {
		if (!strstr($string, $start)) return;
		if (!strstr($string, $stop)) return;
		$string = substr($string, strpos($string,$start)+strlen($start));
		$string = substr($string, 0, strpos($string,$stop));
		return $string;
	}
