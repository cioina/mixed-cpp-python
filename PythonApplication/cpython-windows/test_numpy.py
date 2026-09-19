"""
LazyLoader
"""
def main():
    import sys
    from importlib.util import LazyLoader, find_spec, module_from_spec
    
    # create lazy load of numpy as np
    spec = find_spec("numpy")
    module = module_from_spec(spec)
    sys.modules["numpy"] = module
    loader = LazyLoader(spec.loader)
    loader.exec_module(module)
    np = module
    
    # test a subpackage import
    from numpy.lib import recfunctions  # noqa: F401
    
    # test triggering the import of the package
    np.ndarray

    print(np.__version__)
    # print(np.__all__)
    print(np.pi)
    print(np.e)
    print(np.cos(np.array([np.pi/2,np.radians(90)])))
    print(np.max(np.array([1,2,3,4])))
    
if __name__ == '__main__':
    main()
