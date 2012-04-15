from setuptools import setup, find_packages
import sys, os

setup(name='xflash',
      version='1.3.1',
      description='Xbox360 USB SPI Flasher client',
      long_description='',
      classifiers=[], # Get strings from http://pypi.python.org/pypi?%3Aaction=list_classifiers
      keywords='',
      author='Juvenal',
      author_email='none@of.your.biz',
      url='https://github.com/Juvenal1228/XFlash/',
      license='BSD',
      packages=find_packages(exclude=['ez_setup', 'examples', 'tests']),
      include_package_data=True,
      zip_safe=False,
      install_requires=[
          'pyusb>=1.0.0a1',
          'argparse',
      ],
      entry_points="""
      [console_scripts]
      xflash = xflash:main
      """,
)
