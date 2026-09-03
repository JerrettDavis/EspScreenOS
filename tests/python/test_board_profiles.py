from __future__ import annotations
import json, sys, tempfile, unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools'))
import boardlib

EXPECTED={
 'hosyond-esp32-35-st7796u','hosyond-esp32-40-st7796s','hosyond-esp32-32-st7789p3',
 'diymalls-esp32-2432s032c-i','hosyond-esp32s3-28-ili9341','hosyond-esp32s3-35-st77922'
}
class BoardProfileTests(unittest.TestCase):
 def test_exact_supported_matrix(self): self.assertEqual(set(boardlib.board_ids()),EXPECTED)
 def test_all_profiles_semantically_valid(self):
  for bid in boardlib.board_ids():
   with self.subTest(board=bid):
    findings=boardlib.validate_board(boardlib.load_board(bid));self.assertEqual([], [f for f in findings if f.severity=='error'],findings)
 def test_generated_outputs_are_current(self):
  for bid in boardlib.board_ids():
   ok,findings=boardlib.generate(bid,check=True);self.assertTrue(ok,findings)
 def test_profile_identity_and_provenance(self):
  for bid in boardlib.board_ids():
   b=boardlib.load_board(bid);self.assertEqual(bid,b['id']);self.assertTrue(b['hardware_match']);self.assertTrue(b['source_urls']);self.assertIn(b['maturity'],boardlib.VALID_MATURITY)
 def test_dma_transfer_buffer_is_explicit(self):
  for bid in boardlib.board_ids():self.assertGreater(boardlib.load_board(bid)['display']['transfer_buffer_lines'],0)
 def test_flash_partitions_fit(self):
  for bid in boardlib.board_ids():
   b=boardlib.load_board(bid);csv=(ROOT/'generated'/bid/'partitions.csv').read_text();end=0
   for line in csv.splitlines():
    if not line or line.startswith('#'):continue
    parts=[x.strip() for x in line.split(',')];off=int(parts[3],0);size=int(parts[4],0);end=max(end,off+size)
   self.assertLessEqual(end,b['mcu']['flash_mb']*1024*1024,(bid,hex(end)))
 def test_s3_profiles_use_psram_and_sdmmc(self):
  for bid in [x for x in boardlib.board_ids() if 'esp32s3' in x]:
   b=boardlib.load_board(bid);self.assertGreaterEqual(b['mcu']['psram_mb'],8);self.assertEqual('sdmmc',b['storage']['driver']);self.assertEqual('es8311',b['audio']['driver'])
 def test_motor_pin_is_exclusively_profiled(self):
  b=boardlib._merged(boardlib.load_board('hosyond-esp32s3-35-st77922'))
  self.assertEqual(45,b['haptics']['pin']);self.assertEqual(20000,b['haptics']['pwm_frequency_hz'])
  self.assertNotIn(45,[pin for header in b['headers'] for pin in header.get('pins',[])])
  self.assertNotEqual(46,b['haptics']['pin'])
 def test_classic_profiles_keep_touch_off_dma_spi_hosts(self):
  for bid in [x for x in boardlib.board_ids() if 'esp32-' in x and 'esp32s3' not in x]:self.assertIn(boardlib.load_board(bid)['touch']['bus'],{'soft_spi','i2c'})
 def test_bus_headers_declare_compatible_resource_hosts(self):
  for bid in boardlib.board_ids():
   for header in boardlib.load_board(bid).get('headers',[]):
    with self.subTest(board=bid,header=header.get('name')):
     expected={'i2c':{'i2c0','i2c1'},'spi':{'spi2','spi3'},'uart':{'uart0','uart1','uart2'}}
     if header.get('kind') in expected:self.assertIn(header.get('host'),expected[header['kind']])
  invalid=boardlib.load_board('hosyond-esp32s3-35-st77922')
  invalid['headers'][1].pop('host')
  self.assertTrue(any(f.severity=='error' and 'compatible host' in f.message for f in boardlib.validate_board(invalid)))
 def test_digital_audio_declares_control_and_data_hosts(self):
  for bid in boardlib.board_ids():
   audio=boardlib.load_board(bid)['audio']
   if audio['driver'] in {'es8311','i2s'}:self.assertIn(audio.get('data_host'),{'i2s0','i2s1'},bid)
   if audio['driver']=='es8311':self.assertIn(audio.get('control_host'),{'i2c0','i2c1'},bid)
  invalid=boardlib.load_board('hosyond-esp32s3-35-st77922')
  invalid['audio']['data_host']='spi2'
  findings=boardlib.validate_board(invalid)
  self.assertTrue(any(f.severity=='error' and 'I2S data host' in f.message for f in findings))
 def test_device_bus_hosts_are_semantically_compatible(self):
  for bid in boardlib.board_ids():
   board=boardlib._merged(boardlib.load_board(bid))
   self.assertIn(board['display']['host'],{'spi2','spi3'},bid)
   if board['touch']['bus']=='i2c':self.assertIn(board['touch']['host'],{'i2c0','i2c1'},bid)
   if board['storage']['driver']=='sdspi':self.assertIn(board['storage']['host'],{'spi2','spi3'},bid)
   if board['storage']['driver']=='sdmmc':self.assertEqual(board['storage']['host'],'sdmmc0',bid)
  invalid=boardlib.load_board('hosyond-esp32s3-35-st77922')
  invalid['display']['host']='i2c0'
  invalid['touch']['host']='spi2'
  invalid['storage']['host']='spi3'
  messages=[f.message for f in boardlib.validate_board(invalid) if f.severity=='error']
  self.assertTrue(any('SPI display' in message for message in messages))
  self.assertTrue(any('I2C touch' in message for message in messages))
  self.assertTrue(any('SDMMC storage' in message for message in messages))

if __name__=='__main__':unittest.main()
