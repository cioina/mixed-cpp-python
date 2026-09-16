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

    print(np.pi)
    
if __name__ == '__main__':
    main()
