# -*- coding: utf-8 -*-
# supplementary_order_for_time_integration = 4

f_vol = Variable( interpolation='global', nb_dim=[dim], default_value='0.0,'*(dim-1)+'0.0', unit='N/m^3' )
elastic_modulus = Variable( interpolation='global', default_value='210e9', unit='N/m^2' ) #
poisson_ratio = Variable( interpolation='global', default_value='0.33', unit='1' ) # 
density = Variable( interpolation='global', default_value='7800', unit='kg/m^3' ) # 
relax = Variable( interpolation='global', default_value='1', unit='1' ) # 

dep = Variable( unknown=True, nb_dim=[dim], nb_der=0, default_value='0.0', unit='m' )
lum = Variable( default_value='1.0', unit='1' )

dep_sv = Variable( nb_dim=[dim], default_value='0.0', unit='m' )
dep_sw = Variable( nb_dim=[dim], default_value='0.0', unit='m' )
dep_sx = Variable( nb_dim=[dim], default_value='0.0', unit='m' )

temperature = Variable( default_value='0.0', unit='K' )

sigma      = Variable( interpolation='der_nodal', default_value='0', nb_dim=[dim*(dim+1)/2], unit='N/m^2' )
epsilon    = Variable( interpolation='der_nodal', default_value='0', nb_dim=[dim*(dim+1)/2], unit='1' )
tr_epsilon = Variable( interpolation='der_nodal', default_value='0', unit='1' )
eps_11     = Variable( interpolation='der_nodal', default_value='0', unit='1' )
sigma_p    = Variable( interpolation='elementary', default_value='0', nb_dim=[dim*(dim+1)/2], unit='N/m^2' ) # previous sigma

epsilon_eq = Variable( interpolation='elementary', default_value='0', unit='1' )
epsilon_c = Variable( interpolation='elementary', nb_dim=[dim*(dim+1)/2], default_value='0', unit='1' )
sigma_eq = Variable( interpolation='elementary', default_value='0', unit='1' )

f_nodal = Variable( nb_dim=[dim], nb_der=0, default_value='0.0,'*(dim-1)+'0.0', unit='N' )
f_surf = Variable( interpolation='skin_elementary', nb_dim=[dim], default_value='0.0,'*(dim-1)+'0.0', unit='N/m^2' )
p = Variable( interpolation='skin_elementary', default_value='0.0', unit='N/m^2' )

normal = Variable( nb_dim=[dim], default_value='0.0', unit='1' )

# dirichlet
dep_imp_val  = Variable( nb_dim=[dim], default_value='0', unit='1' )
dep_imp_coef = Variable( nb_dim=[dim], default_value='0', unit='1' )

#neig_pointer = Variable( interpolation='elementary', T='Vec<EA *,2>', default_value='',  unit='', dont_use_caracdm = True )

integration_totale = False
# assume_symmetric_matrix = False
assume_linear_system = False
assume_non_linear = True

#use_asm = True
sigma_0   = Variable( interpolation='global'    , default_value='200e6', unit='N/m^2' )
n         = Variable( interpolation='global'    , default_value='5'    , unit='1' )

poisson_ratio_sens = Variable( interpolation='global', default_value='0', unit='1' ) #
sigma_0_sens       = Variable( interpolation='global', default_value='0', unit='1' ) #
n_sens             = Variable( interpolation='global', default_value='0', unit='1' ) #
param              = Variable( interpolation='global', default_value='0', unit='1' ) #
    
# --------------------------------------------------------------------------------------------------------------------------------
def formulation():
    def get_d_sig():
        E  = elastic_modulus.expr
        nu = poisson_ratio.expr + poisson_ratio_sens.expr * param.expr
        epsilon = grad_sym_col( dep.expr )
        np = n.expr + n_sens.expr * param.expr
        s0 = sigma_0.expr + sigma_0_sens.expr * param.expr

        sig = sigma_p.expr
        eqs = ExVector( dim * ( dim + 1 ) / 2 )
        for i in range( dim * ( dim + 1 ) / 2 ):
            t = sig[ i ] - trace( sig ) * ( i < dim ) / dim
            eqs[ i ] = ( 1 + nu ) / E * sig[ i ] - nu / E * trace( sig ) * ( i < dim ) - epsilon[ i ] + \
                1.5 * ( von_mises( sig, 1e-6 ) / s0 ) ** ( np - 1 ) * t / E

        M = ExMatrix( 3, 3 )
        for i in range( dim * ( dim + 1 ) / 2 ):
            for j in range( dim * ( dim + 1 ) / 2 ):
                M[ j, i ] = eqs[ i ].diff( sig[ j ] )
                #M[ i, j ] = eqs[ i ].diff( sig[ j ] )

        return M.solve( eqs )
    
    epsilon = grad_sym_col( dep.expr )
    epstest = grad_sym_col( dep.test )

    d_sig = get_d_sig()

    sigma = ExVector( dim * ( dim + 1 ) / 2 )
    for i in range( dim * ( dim + 1 ) / 2 ):
        sigma[ i ] = sigma_p.expr[ i ] - d_sig[ i ]

    res = trace_sym_col( sigma, epstest )
    
    dmp = dot( dep.expr - dep_imp_val.expr, dep_imp_coef.expr * dep.test )
    
    return res * dV - dot( f_nodal.expr, dep.test ) * dN - dot( f_surf.expr, dep.test ) * dS - dot( p.expr * dS_normal, dep.test ) * dS + dmp * dN

# --------------------------------------------------------------------------------------------------------------------------------
def apply_on_elements_after_solve( unk_subs ): # return a string
    def get_d_sig():
        E  = elastic_modulus.expr
        nu = poisson_ratio.expr + poisson_ratio_sens.expr * param.expr
        epsilon = grad_sym_col( dep.expr )
        np = n.expr + n_sens.expr * param.expr
        s0 = sigma_0.expr + sigma_0_sens.expr * param.expr

        sig = sigma_p.expr
        eqs = ExVector( dim * ( dim + 1 ) / 2 )
        for i in range( dim * ( dim + 1 ) / 2 ):
            t = sig[ i ] - trace( sig ) * ( i < dim ) / dim
            eqs[ i ] = ( 1 + nu ) / E * sig[ i ] - nu / E * trace( sig ) * ( i < dim ) - epsilon[ i ] + \
                1.5 * (
                    von_mises( sig, 1e-6 ) / s0
                ) ** ( np - 1 ) * t / E

        M = ExMatrix( 3, 3 )
        for i in range( dim * ( dim + 1 ) / 2 ):
            for j in range( dim * ( dim + 1 ) / 2 ):
                M[ j, i ] = eqs[ i ].diff( sig[ j ] )
                #M[ i, j ] = eqs[ i ].diff( sig[ j ] )

        return M.solve( eqs )

    d_sig = get_d_sig()
    epsilon = grad_sym_col( dep.expr )
    epsilon_eq = von_mises( epsilon )
    sigma_eq   = von_mises( sigma_p.expr )

    my_subs = unk_subs
    my_subs[ time ] = time_steps[ 0 ]
    for vi in e.var_inter: my_subs[ vi ] = number( 0.5 )

    d_sig      = d_sig     .subs(EM(my_subs))
    epsilon    = epsilon   .subs(EM(my_subs))
    epsilon_eq = epsilon_eq.subs(EM(my_subs))
    sigma_eq   = sigma_eq  .subs(EM(my_subs))

    cw = Write_code('T')
    for i in range( dim * ( dim + 1 ) / 2 ):
        cw.add( d_sig[ i ], 'd_sigma_p_' + str( i ), Write_code.Declare )
        cw.add( epsilon[ i ], 'elem.epsilon[0][' + str( i ) + ']', Write_code.Set )
	cw.add( epsilon[ i ], 'elem.epsilon_c [' + str( i ) + ']', Write_code.Set )
    cw.add( epsilon_eq, 'elem.epsilon_eq', Write_code.Set )
    cw.add( sigma_eq  , 'elem.sigma_eq'  , Write_code.Set )
    
    res = cw.to_string()
    for i in range( dim * ( dim + 1 ) / 2 ):
        res += '    elem.sigma_p[' + str( i ) + '] -= f.m->relax * d_sigma_p_' + str( i ) + ';\n'
    
    return res
