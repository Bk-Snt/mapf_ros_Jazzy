from setuptools import find_packages
from setuptools import setup

setup(
    name='mapf_msgs',
    version='2.0.0',
    packages=find_packages(
        include=('mapf_msgs', 'mapf_msgs.*')),
)
