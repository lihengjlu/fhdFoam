Info << "Reading transportProperties" << nl;

IOdictionary transportProperties
(
    IOobject
    (
        "transportProperties",
        runTime.constant(),
        mesh,
        IOobject::MUST_READ,
        IOobject::NO_WRITE
    )
);

dimensionedScalar nu
(
    "nu",
    dimensionSet(0,  2, -1, 0, 0, 0, 0),
    transportProperties
);

dimensionedScalar T
(
    "T",
    dimensionSet(0,  0,  0, 1, 0, 0, 0),
    transportProperties
);

dimensionedScalar rho
(
    "rho",
    dimensionSet(1, -3,  0, 0, 0, 0, 0),
    transportProperties
);

dimensionedScalar td
(
    "td",
    dimensionSet(0, 0, 1, 0, 0, 0, 0),
    transportProperties
);

dimensionedScalar h0
(
    "h0",
    dimensionSet(0, 1,  0, 0, 0, 0, 0),
    transportProperties
);

// *****

dimensionedScalar D
(
    "D",
    dimensionSet(0, 2, -1, 0, 0, 0, 0),
    transportProperties
);

dimensionedScalar M
(
    "M",
    dimensionSet(1, 0,  0, 0, -1, 0, 0),
    transportProperties
);

dimensionedScalar c_in
(
    "c_in",
    dimensionSet(0, -3,  0, 0, 1, 0, 0),
    transportProperties
);

dimensionedScalar c_s
(
    "c_s",
    dimensionSet(0, -3,  0, 0, 1, 0, 0),
    transportProperties
);

// *****

dimensionedScalar k
(
    "k",
    dimensionSet(0, -2, -1, 0, 1, 0, 0),
    transportProperties
);

dimensionedScalar nu_m
(
    "nu_m",
    dimensionSet(0, 3,  0, 0, -1, 0, 0),
    transportProperties
);


Info << nl;
Info << "***************************************************" << nl;
Info << "kinematic viscosity, nu:     " << nu  <<  " cm^2/s"     << nl;
Info << "temperature, T:              " << T   <<  " K"          << nl;
Info << "density of solvent, rho:     " << rho <<  " g/cm^3"     << nl;
Info << "unit time, td:               " << td  <<  " s"          << nl;
Info << "unit length,   h0:           " << h0  <<  " cm"         << nl;
Info << nl;
Info << "molecular diffusion, D:      " << D   <<  " cm^2/s"     << nl;
Info << "molar mass, M:               " << M   <<  " g/mol"      << nl;
Info << "inlet concentration, c_in:   " << c_in<<  " mol/L"      << nl;
Info << "equilibrium conc., c_s:      " << c_s <<  " mol/L"      << nl;
Info << nl;
Info << "reaction rate constant, k:   " << k   <<  " mol/cm^2/s" << nl;
Info << "molar vol. of mineral, nu_m: " << nu_m<<  " cm^3/mol"   << nl;
Info << "***************************************************" << endl;

// convert concentrations from mol/L to mol/cm^3
c_in *= 0.001;
c_s  *= 0.001;

dimensionedScalar k_B
(
    "k_B",
    dimensionSet(1, 2, -2, -1, 0, 0, 0),
    k_Boltzmann
);
dimensionedScalar N_A
(
    "N_A",
    dimensionSet(0, 0, 0, 0, -1, 0, 0),
    Avogadro
);


Info<<nl<<"Calculated dimensionless numbers"<<nl;

// here the viscosity, for example is still dimensionedScalar
// in order to overcome OF's dimension check.
dimensionedScalar nu_h_dimless = nu * td / (h0*h0);

dimensionedScalar nu_h
(
    "nu_h",
    dimensionSet(0, 2, -1, 0, 0, 0, 0),
    nu_h_dimless.value()
);

dimensionedScalar D_h_dimless = D * td / (h0*h0);
dimensionedScalar D_h
(
    "D_h",
    dimensionSet(0, 2, -1, 0, 0, 0, 0),
    D_h_dimless.value()
);

dimensionedScalar ccc = c_s / c_in;

// theta is 1/ccc, which is c_in/c_s
dimensionedScalar theta
(
    "theta",
    dimensionSet(0, 0, 0, 0, 0, 0, 0),
    transportProperties
);

dimensionedScalar Pe
(
    "Pe",
    dimensionSet(0, 0, 0, 0, 0, 0, 0),
    transportProperties
);

dimensionedScalar DaII
(
    "DaII",
    dimensionSet(0, 0, 0, 0, 0, 0, 0),
    ((k * h0) / (D * c_in )).value()
);

dimensionedScalar DaI
(
    "DaI",
    dimensionSet(0, 0, 0, 0, 0, 0, 0),
    (DaII / Pe).value()
);

Info << "nu_h:     " << nu_h_dimless    << nl;
Info << "D_h:      " << D_h_dimless     << nl;

Info << "U0:      " << h0.value()/td.value()<<nl;

Info << "Pe:   " << Pe <<nl;
Info << "DaI:  " << DaI <<nl;
Info << "DaII: " << DaII<<nl;

dimensionedScalar gamma_p
(
    "gamma_p",
    dimensionSet(0, 0, 0, 0, 0, 0, 0),
    transportProperties
);

dimensionedScalar factor_drdt
(
    "factor_drdt",
    dimensionSet(0, 0, 0, 0, 0, 0, 0),
    transportProperties
);

dimensionedScalar l_R
(
    "l_R",
    dimensionSet(0, 0, 0, 0, 0, 0, 0),
    transportProperties
);

Info << "l_R:         " << l_R << nl;
Info << "factor_drdt: " << factor_drdt << nl;

// additional parameters for nucleation and scaling the noise
bool nucleation = transportProperties.getOrDefault("nucleation", false);

Info<<"nucleation: "<<nucleation<<nl;


// TODO some parameters for nucleation will be hardcoded for now
scalar gamma = 35. * Foam::pow(10., -7);

scalar kBT = (k_B * T).value();
scalar nua = (nu_m / N_A).value();
scalar Bcoef = 16./3. * Foam::constant::mathematical::pi * Foam::pow(nua, 2) * Foam::pow(gamma/kBT, 3);

//scalar lnA = 15.0;
scalar lnA = transportProperties.getOrDefault("lnA", 0.0);

Info<<"lnA: "<< lnA <<"   Bcoef: "<<Bcoef<<nl<<nl;

